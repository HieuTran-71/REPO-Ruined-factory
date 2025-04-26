#include "MainMenu.h"
#include "TextOb.h"
#include <iostream>
#include "CommonFunc.h"


#define HIGHSCORE_FILE "highscore.txt"

#define HIGHSCORE_FILE "highscore.txt"

#define GAME_OVER_WIDTH 400
#define GAME_OVER_HEIGHT 200
#define BUTTON_WIDTH 120
#define BUTTON_HEIGHT 40
#define EXIT_BUTTON_X (GAME_OVER_WIDTH / 4 - BUTTON_WIDTH / 2)
#define CONTINUE_BUTTON_X (GAME_OVER_WIDTH * 3/4 - BUTTON_WIDTH / 2)
#define BUTTON_Y (GAME_OVER_HEIGHT - BUTTON_HEIGHT - 20)
#define CLOSE_BUTTON_X (GAME_OVER_WIDTH - 30) // X position of the close button
#define CLOSE_BUTTON_Y 10                      // Y position of the close button
#define CLOSE_BUTTON_SIZE 20

MainMenu::MainMenu() : selected_index_(0), font_(nullptr), high_score_(0), game_over_(false) {} // Thêm biến game_over_

MainMenu::~MainMenu() {
    if (font_ != nullptr) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }
     if (game_over_texture_ != nullptr) { // Giải phóng texture
        SDL_DestroyTexture(game_over_texture_);
        game_over_texture_ = nullptr;
    }
}

bool MainMenu::LoadMenu(SDL_Renderer* screen) {
    // Nạp ảnh nền
    background_.LoadImg("image/menu_background.png", screen);

    // Nạp font
    font_ = TTF_OpenFont("font/CyberpunkCraftpixPixel.otf", 28);
    if (font_ == nullptr) {
        std::cerr << "Không thể tải font: " << TTF_GetError() << std::endl;
        return false; // Trả về false nếu không tải được font
    }
    options_ = {"Play Game", "High score", "Exit"};

    LoadHighScore(); // Đọc high score tốt nhất
    highScoreText_.SetColor(TextOb::RED_TEXT);
    highScoreText_.SetFont(font_);
    std::string highScoreStr = "High Score: " + std::to_string(high_score_);
    highScoreText_.SetText(highScoreStr);
    highScoreText_.LoadFromRenderText(font_, screen);

    g_screen = screen; // Lưu lại renderer để dùng sau này.

     // Load texture cho cửa sổ Game Over
    SDL_Surface* game_over_surface = IMG_Load("image/game_over_window.png"); // Thay thế bằng đường dẫn hình ảnh của bạn
    if (game_over_surface == nullptr) {
        std::cerr << "Không thể tải ảnh Game Over: " << SDL_GetError() << std::endl;
        // Không trả về false ở đây, vì game có thể tiếp tục chạy mà không có cửa sổ Game Over.
    } else {
        game_over_texture_ = SDL_CreateTextureFromSurface(screen, game_over_surface);
        SDL_FreeSurface(game_over_surface);
        if (game_over_texture_ == nullptr) {
             std::cerr << "Không thể tạo texture Game Over: " << SDL_GetError() << std::endl;
        }
    }

    return (font_ != nullptr);
}

void MainMenu::LoadHighScore() {
    std::ifstream file(HIGHSCORE_FILE);
    high_scores_.clear(); // Xóa dữ liệu cũ
    if (file.is_open()) {
        int score;
        while (file >> score) {
            high_scores_.push_back(score);
        }
        file.close();
    } else {
        std::cerr << "Không thể mở file " << HIGHSCORE_FILE << " để đọc!\n";
        // Nếu không mở được file, tạo một file mới.
        std::ofstream createFile(HIGHSCORE_FILE);
        if (createFile.is_open())
        {
            createFile.close();
        }
    }
    // Đảm bảo vector không vượt quá 10 phần tử
    if (high_scores_.size() > 10) {
        high_scores_.resize(10);
    }
}

void MainMenu::SaveHighScore(int current_mark) {
    high_scores_.push_back(current_mark);
    std::sort(high_scores_.begin(), high_scores_.end(), std::greater<int>()); // Sắp xếp giảm dần
    if (high_scores_.size() > 10) {
        high_scores_.resize(10); // Giữ tối đa 10 giá trị
    }

    //Ghi vào file
    std::ofstream file(HIGHSCORE_FILE);
    if (file.is_open()) {
        for (int score : high_scores_) {
            file << score << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Không thể mở file " << HIGHSCORE_FILE << " để ghi!\n";
    }
    // Load lại để cập nhật hiển thị
    LoadHighScore();
    //Cập nhật hiển thị
    std::string highScoreStr = "High Score: " + std::to_string(high_scores_[0]);
    highScoreText_.SetText(highScoreStr);
    highScoreText_.LoadFromRenderText(font_, g_screen); // Cần g_screen
}

void MainMenu::ShowHighScoreBoard(SDL_Renderer* screen) {
    bool show_score = true;
    SDL_Event e;

    // Load font
    TTF_Font* font = TTF_OpenFont("font/CyberpunkCraftpixPixel.otf", 32); //Sửa lại font chữ
    if (font == NULL) {
        std::cerr<< "Failed to load font!" << SDL_GetError() << std::endl;
        return;
    }

    while (show_score) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                show_score = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) { // Nhấn ESC để quay lại
                    show_score = false;
                }
            }
        }

        SDL_SetRenderDrawColor(screen, 0, 0, 0, 255); // Màu nâu
        SDL_RenderClear(screen);

        // Tiêu đề
        TextOb title;
        title.SetText("High Score");
        title.SetColor(TextOb::WHITE_TEXT);
        title.SetFont(font);
        title.LoadFromRenderText(font, screen);
        title.RenderText(screen, SCREEN_WIDTH / 2 - title.GetWidth()/2, 50);

        // Hiển thị 10 điểm cao
        int startY = 120;
        for (size_t i = 0; i < high_scores_.size(); ++i) {
            std::string rank_text = "No " + std::to_string(i + 1) + " :";
            TextOb rank_item;
            rank_item.SetText(rank_text);
            rank_item.SetColor(TextOb::WHITE_TEXT);
            rank_item.SetFont(font);
            rank_item.LoadFromRenderText(font, screen);
            rank_item.RenderText(screen, SCREEN_WIDTH / 2 - 150, startY);

            std::string score_text = std::to_string(high_scores_[i]);
            TextOb score_item;
            score_item.SetText(score_text);
            score_item.SetColor(TextOb::WHITE_TEXT);
            score_item.SetFont(font);
            score_item.LoadFromRenderText(font, screen);
            SDL_Rect scoreRect = { SCREEN_WIDTH / 2 - 30, startY - 5, 100, score_item.GetHeight() + 10 }; // Điều chỉnh kích thước
            SDL_SetRenderDrawColor(screen, 255, 255, 255, 255); // Màu trắng
            SDL_RenderDrawRect(screen, &scoreRect);
            score_item.RenderText(screen, SCREEN_WIDTH / 2 - 20, startY);
            startY += 50;
        }

        // Ghi chú
        TextOb note;
        note.SetText("Press Esc to comeback");
        note.SetColor(TextOb::WHITE_TEXT);
        note.SetFont(font);
        note.LoadFromRenderText(font, screen);
        note.RenderText(screen, SCREEN_WIDTH / 2 - note.GetWidth()/2, SCREEN_HEIGHT - 60);

        SDL_RenderPresent(screen);
    }
    TTF_CloseFont(font);
}


void MainMenu::Render(SDL_Renderer* screen) {
    background_.Render(screen, nullptr);

    // Vị trí tiêu đề
    int title_y = 100;

    // Hiển thị tiêu đề với khung
    TextOb title_text;
    title_text.SetText("R.O.B.O : Ruined Factory");
    title_text.SetColor(TextOb::WHITE_TEXT);
    title_text.SetFont(font_);
    title_text.LoadFromRenderText(font_, screen);

    // Tính toán khung cho tiêu đề
    int title_x = SCREEN_WIDTH / 2 - title_text.GetWidth() / 2;
    SDL_Rect title_frame = {
        title_x - 20,
        title_y - 15,
        title_text.GetWidth() + 40,
        title_text.GetHeight() + 30
    };

    // Vẽ nền cho khung tiêu đề
    SDL_SetRenderDrawColor(screen, 0, 0, 80, 200); // Màu xanh đậm, hơi trong suốt
    SDL_SetRenderDrawBlendMode(screen, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(screen, &title_frame);

    // Vẽ khung tiêu đề
    SDL_SetRenderDrawColor(screen, 255, 255, 255, 255); // Màu trắng
    SDL_RenderDrawRect(screen, &title_frame);

    // Render text tiêu đề
    title_text.RenderText(screen, title_x, title_y);

    // Tính toán chiều rộng khung tối đa cho các tùy chọn
    int max_option_width = 0;
    for (const auto& option : options_) {
        TextOb temp_text;
        temp_text.SetText(option);
        temp_text.SetFont(font_);
        temp_text.LoadFromRenderText(font_, screen);
        if (temp_text.GetWidth() > max_option_width) {
            max_option_width = temp_text.GetWidth();
        }
    }
    max_option_width += 30;

    // Options
    int option_start_y = title_y + 120;
    for (size_t i = 0; i < options_.size(); ++i) {
        TextOb option_text;
        option_text.SetText(options_[i]);
        option_text.SetFont(font_);

        // Màu text tùy thuộc vào lựa chọn
        if (i == selected_index_) {
            option_text.SetColor(TextOb::RED_TEXT);
        } else {
            option_text.SetColor(TextOb::WHITE_TEXT);
        }

        option_text.LoadFromRenderText(font_, screen);

        int option_y = option_start_y + i * 60;
        int option_x = SCREEN_WIDTH / 2 - option_text.GetWidth() / 2;

        // Vẽ khung cho tùy chọn với chiều rộng đồng nhất
        SDL_Rect option_frame = {
            SCREEN_WIDTH / 2 - max_option_width / 2,
            option_y - 10,
            max_option_width,
            option_text.GetHeight() + 20
        };

        // Đổ màu nền cho tùy chọn
        if (i == selected_index_) {
            // Nền màu đỏ nhạt cho tùy chọn được chọn
            SDL_SetRenderDrawColor(screen, 100, 0, 0, 180);
        } else {
            // Nền tối cho các tùy chọn khác
            SDL_SetRenderDrawColor(screen, 20, 20, 60, 180);
        }
        SDL_RenderFillRect(screen, &option_frame);

        // Vẽ viền cho tất cả các tùy chọn (kiểu dáng giống nhau)
        SDL_SetRenderDrawColor(screen, 255, 255, 255, 255); // Màu trắng
        SDL_RenderDrawRect(screen, &option_frame);

        // Render text ở giữa khung
        option_text.RenderText(screen, option_x, option_y);
    }

     if (game_over_ && game_over_texture_ != nullptr) { // Kiểm tra nếu game_over_ là true và texture tồn tại
        // Vẽ cửa sổ Game Over
        SDL_Rect destRect;
        destRect.x = (SCREEN_WIDTH - GAME_OVER_WIDTH) / 2;
        destRect.y = (SCREEN_HEIGHT - GAME_OVER_HEIGHT) / 2;
        destRect.w = GAME_OVER_WIDTH;
        destRect.h = GAME_OVER_HEIGHT;
        SDL_RenderCopy(screen, game_over_texture_, NULL, &destRect);

        // Vẽ nút "Exit" màu đỏ
        SDL_Rect exitButtonRect = {
            destRect.x + EXIT_BUTTON_X,
            destRect.y + BUTTON_Y,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        SDL_SetRenderDrawColor(screen, 255, 0, 0, 255); // Màu đỏ
        SDL_RenderFillRect(screen, &exitButtonRect);

        // Vẽ chữ "Exit" lên nút
        TextOb exitText;
        exitText.SetText("Exit");
        exitText.SetColor(TextOb::WHITE_TEXT);
        exitText.SetFont(font_);
        exitText.LoadFromRenderText(font_, screen);
        exitText.RenderText(screen, exitButtonRect.x + (BUTTON_WIDTH - exitText.GetWidth()) / 2, exitButtonRect.y + (BUTTON_HEIGHT - exitText.GetHeight()) / 2);

        // Vẽ nút "Continue" màu xanh lá cây
        SDL_Rect continueButtonRect = {
            destRect.x + CONTINUE_BUTTON_X,
            destRect.y + BUTTON_Y,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        SDL_SetRenderDrawColor(screen, 0, 255, 0, 255); // Màu xanh lá cây
        SDL_RenderFillRect(screen, &continueButtonRect);

        // Vẽ chữ "Continue" lên nút
        TextOb continueText;
        continueText.SetText("Continue");
        continueText.SetColor(TextOb::WHITE_TEXT);
        continueText.SetFont(font_);
        continueText.LoadFromRenderText(font_, screen);
        continueText.RenderText(screen, continueButtonRect.x + (BUTTON_WIDTH - continueText.GetWidth()) / 2, continueButtonRect.y + (BUTTON_HEIGHT - continueText.GetHeight()) / 2);

        // Draw the close button (red X)
        SDL_Rect closeButtonRect = {
            destRect.x + CLOSE_BUTTON_X,
            destRect.y + CLOSE_BUTTON_Y,
            CLOSE_BUTTON_SIZE,
            CLOSE_BUTTON_SIZE
        };
        SDL_SetRenderDrawColor(screen, 255, 0, 0, 255); // Red color for the X
        SDL_RenderDrawLine(screen, closeButtonRect.x, closeButtonRect.y, closeButtonRect.x + closeButtonRect.w, closeButtonRect.y + closeButtonRect.h);
        SDL_RenderDrawLine(screen, closeButtonRect.x + closeButtonRect.w, closeButtonRect.y, closeButtonRect.x, closeButtonRect.y + closeButtonRect.h);
    }
}

// Cần điều chỉnh cả hàm HandleEvent để sử dụng cùng kích thước khung
int MainMenu::HandleEvent(SDL_Event& event, int& current_mark) {
     if (game_over_) { // Xử lý sự kiện Game Over
        int mouse_x, mouse_y;
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            mouse_x = event.button.x;
            mouse_y = event.button.y;

            // Tính toán vị trí cửa sổ Game Over
            int game_over_x = (SCREEN_WIDTH - GAME_OVER_WIDTH) / 2;
            int game_over_y = (SCREEN_HEIGHT - GAME_OVER_HEIGHT) / 2;

             // Kiểm tra nút Exit
            SDL_Rect exitButtonRect = {
                game_over_x + EXIT_BUTTON_X,
                game_over_y + BUTTON_Y,
                BUTTON_WIDTH,
                BUTTON_HEIGHT
            };
            if (mouse_x >= exitButtonRect.x && mouse_x <= exitButtonRect.x + exitButtonRect.w &&
                mouse_y >= exitButtonRect.y && mouse_y <= exitButtonRect.y + exitButtonRect.h) {
                return MENU_EXIT; // Thoát game
            }

            // Kiểm tra nút Continue
            SDL_Rect continueButtonRect = {
                game_over_x + CONTINUE_BUTTON_X,
                game_over_y + BUTTON_Y,
                BUTTON_WIDTH,
                BUTTON_HEIGHT
            };
            if (mouse_x >= continueButtonRect.x && mouse_x <= continueButtonRect.x + continueButtonRect.w &&
                mouse_y >= continueButtonRect.y && mouse_y <= continueButtonRect.y + continueButtonRect.h) {
                game_over_ = false; // Ẩn cửa sổ Game Over
                return MENU_NONE;    // Trở về menu chính
            }

             // Kiểm tra nút Close (X)
            SDL_Rect closeButtonRect = {
                game_over_x + CLOSE_BUTTON_X,
                game_over_y + CLOSE_BUTTON_Y,
                CLOSE_BUTTON_SIZE,
                CLOSE_BUTTON_SIZE
            };
            if (mouse_x >= closeButtonRect.x && mouse_x <= closeButtonRect.x + closeButtonRect.w &&
                mouse_y >= closeButtonRect.y && mouse_y <= closeButtonRect.y + closeButtonRect.h) {
                game_over_ = false; // Hide the Game Over window
                return MENU_NONE;    // Return to the main menu
            }
        }
        return MENU_NONE; // Không xử lý thêm sự kiện nào khác khi đang ở màn hình Game Over
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mouse_x = event.button.x;
        int mouse_y = event.button.y;

        int option_start_y = 220; // Vị trí y bắt đầu của tùy chọn đầu tiên

        // Tính toán chiều rộng khung tối đa cho các tùy chọn
        int max_option_width = 0;
        for (const auto& option : options_) {
            int text_width = 0;
            int text_height = 0;
            TTF_SizeText(font_, option.c_str(), &text_width, &text_height);
            if (text_width > max_option_width) {
                max_option_width = text_width;
            }
        }
        max_option_width += 30;

        for (size_t i = 0; i < options_.size(); ++i) {
            int text_height = 0;
            TTF_SizeText(font_, options_[i].c_str(), NULL, &text_height);

            // Vùng nhấp chọn với chiều rộng đồng nhất
            SDL_Rect rect = {
                SCREEN_WIDTH / 2 - max_option_width / 2,
                option_start_y + i * 60 - 10,
                max_option_width,
                text_height + 20
            };

            if (mouse_x >= rect.x && mouse_x <= rect.x + rect.w &&
                mouse_y >= rect.y && mouse_y <= rect.y + rect.h) {
                if (i == 0) return MENU_PLAY;
                if (i == 1) return MENU_HIGHSCORE;
                if (i == 2) return MENU_EXIT;
            }
        }
    }

    // Xử lý sự kiện di chuyển chuột để highlight menu item
    if (event.type == SDL_MOUSEMOTION) {
        int mouse_x = event.motion.x;
        int mouse_y = event.motion.y;

        int option_start_y = 220; // Vị trí y bắt đầu của tùy chọn đầu tiên

        // Tính toán chiều rộng khung tối đa cho các tùy chọn
        int max_option_width = 0;
        for (const auto& option : options_) {
            int text_width = 0;
            int text_height = 0;
            TTF_SizeText(font_, option.c_str(), &text_width, &text_height);
            if (text_width > max_option_width) {
                max_option_width = text_width;
            }
        }
        max_option_width += 30;

        for (size_t i = 0; i < options_.size(); ++i) {
            int text_height = 0;
            TTF_SizeText(font_, options_[i].c_str(), NULL, &text_height);

            // Vùng phát hiện với chiều rộng đồng nhất
            SDL_Rect rect = {
                SCREEN_WIDTH / 2 - max_option_width / 2,
                option_start_y + i * 60 - 10,
                max_option_width,
                text_height + 20
            };

            if (mouse_x >= rect.x && mouse_x <= rect.x + rect.w &&
                mouse_y >= rect.y && mouse_y <= rect.y + rect.h) {
                selected_index_ = i;
                break;
            }
        }
    }

    // Thêm xử lý phím di chuyển
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                selected_index_ = (selected_index_ > 0) ? selected_index_ - 1 : options_.size() - 1;
                break;
            case SDLK_DOWN:
                selected_index_ = (selected_index_ < options_.size() - 1) ? selected_index_ + 1 : 0;
                break;
            case SDLK_RETURN:
                if (selected_index_ == 0) return MENU_PLAY;
                if (selected_index_ == 1)
                {
                    ShowHighScoreBoard(g_screen);
                    return MENU_NONE;
                }
                if (selected_index_ == 2) return MENU_EXIT;
                break;
            default:
                break;
        }
    }

    return MENU_NONE;
}

void MainMenu::SetGameOver(bool game_over) {
    game_over_ = game_over;
}
