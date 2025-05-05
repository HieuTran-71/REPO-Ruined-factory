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
    font_ = TTF_OpenFont("font/CyberpunkCraftpixPixel.otf", 36);
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
    Uint32 lastTime = SDL_GetTicks();

    // Load fonts with different sizes
    TTF_Font* titleFont = TTF_OpenFont("font/CyberpunkCraftpixPixel.otf", 40);
    TTF_Font* scoreFont = TTF_OpenFont("font/CyberpunkCraftpixPixel.otf", 26);
    TTF_Font* noteFont = TTF_OpenFont("font/CyberpunkCraftpixPixel.otf", 20);

    if (titleFont == NULL || scoreFont == NULL || noteFont == NULL) {
        std::cerr << "Failed to load fonts! " << SDL_GetError() << std::endl;
        return;
    }


    // Animation variables
    float animOffset = 0.0f;
    bool animDirection = true;

    while (show_score) {
        // Calculate time delta for animations
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Update animations
        animOffset += (animDirection ? 1 : -1) * deltaTime * 30.0f;
        if (animOffset > 10.0f) animDirection = false;
        if (animOffset < -10.0f) animDirection = true;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                show_score = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    show_score = false;
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(screen, 0, 0, 0, 255);
        SDL_RenderClear(screen);

        // Gradient background if image not available
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            int r = 20 + (y * 40 / SCREEN_HEIGHT);
            int g = 10 + (y * 20 / SCREEN_HEIGHT);
            int b = 40 + (y * 60 / SCREEN_HEIGHT);
            SDL_SetRenderDrawColor(screen, r, g, b, 255);
            SDL_RenderDrawLine(screen, 0, y, SCREEN_WIDTH, y);
        }


        // Decorative border
        SDL_Rect borderRect = {20, 20, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 40};
        SDL_SetRenderDrawColor(screen, 200, 180, 100, 255);
        SDL_RenderDrawRect(screen, &borderRect);

        SDL_Rect innerBorderRect = {25, 25, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50};
        SDL_SetRenderDrawColor(screen, 150, 130, 80, 255);
        SDL_RenderDrawRect(screen, &innerBorderRect);

        // Score table header
        SDL_Rect headerRect = {SCREEN_WIDTH / 2 - 200, 60, 400, 40};
        SDL_SetRenderDrawColor(screen, 80, 60, 120, 200);
        SDL_RenderFillRect(screen, &headerRect);
        SDL_SetRenderDrawColor(screen, 200, 200, 200, 255);
        SDL_RenderDrawRect(screen, &headerRect);

        TextOb rankHeader;
        rankHeader.SetFont(scoreFont);
        rankHeader.SetText("RANK");
        rankHeader.SetColor(TextOb::YELLOW_TEXT);
        rankHeader.LoadFromRenderText(scoreFont, screen);
        rankHeader.RenderText(screen, SCREEN_WIDTH / 2 - 170, 65);

        TextOb scoreHeader;
        scoreHeader.SetFont(scoreFont);
        scoreHeader.SetText("SCORE");
        scoreHeader.SetColor(TextOb::YELLOW_TEXT);
        scoreHeader.LoadFromRenderText(scoreFont, screen);
        scoreHeader.RenderText(screen, SCREEN_WIDTH / 2 + 50, 65);

        // Display scores with visual enhancements
        int startY = 110;
        for (size_t i = 0; i < high_scores_.size() && i < 10; ++i) {
            // Row background (alternating colors)
            SDL_Rect rowRect = {SCREEN_WIDTH / 2 - 200, startY - 5, 400, 45};
            if (i % 2 == 0) {
                SDL_SetRenderDrawColor(screen, 40, 40, 60, 150);
            } else {
                SDL_SetRenderDrawColor(screen, 50, 50, 70, 150);
            }
            SDL_RenderFillRect(screen, &rowRect);

            // Highlight for top 3 scores
            if (i < 3) {
                SDL_Rect highlightRect = {SCREEN_WIDTH / 2 - 205, startY - 10, 410, 55};
                if (i == 0) {
                    SDL_SetRenderDrawColor(screen, 255, 215, 0, 100); // Gold
                } else if (i == 1) {
                    SDL_SetRenderDrawColor(screen, 192, 192, 192, 100); // Silver
                } else {
                    SDL_SetRenderDrawColor(screen, 205, 127, 50, 100); // Bronze
                }
                SDL_RenderDrawRect(screen, &highlightRect);
            }

            // Rank display
            std::string rank_text = "#" + std::to_string(i + 1);
            TextOb rank_item;
            rank_item.SetFont(scoreFont);
            rank_item.SetText(rank_text);

            // Different colors for different ranks
            if (i == 0) {
                rank_item.SetColor(255, 215, 0); // Gold
            } else if (i == 1) {
                rank_item.SetColor(220, 220, 220); // Silver
            } else if (i == 2) {
                rank_item.SetColor(205, 127, 50); // Bronze
            } else {
                rank_item.SetColor(TextOb::WHITE_TEXT);
            }

            rank_item.LoadFromRenderText(scoreFont, screen);
            rank_item.RenderText(screen, SCREEN_WIDTH / 2 - 170, startY);

            // Score display with box
            std::string score_text = std::to_string(high_scores_[i]);
            TextOb score_item;
            score_item.SetFont(scoreFont);
            score_item.SetText(score_text);

            // Different colors for different ranks
            if (i == 0) {
                score_item.SetColor(255, 255, 150); // Gold
            } else if (i == 1) {
                score_item.SetColor(230, 230, 230); // Silver
            } else if (i == 2) {
                score_item.SetColor(255, 200, 150); // Bronze
            } else {
                score_item.SetColor(TextOb::WHITE_TEXT);
            }

            score_item.LoadFromRenderText(scoreFont, screen);

            // Score box with slight glow for top scores
            SDL_Rect scoreRect = {SCREEN_WIDTH / 2 + 50, startY - 5, 120, 40};

            if (i < 3) {
                // Glow effect for top 3
                SDL_Rect glowRect = {scoreRect.x - 3, scoreRect.y - 3, scoreRect.w + 6, scoreRect.h + 6};
                if (i == 0) {
                    SDL_SetRenderDrawColor(screen, 255, 215, 0, 100 + 50 * sin(currentTime/300.0f));
                } else if (i == 1) {
                    SDL_SetRenderDrawColor(screen, 192, 192, 192, 100 + 30 * sin(currentTime/400.0f));
                } else {
                    SDL_SetRenderDrawColor(screen, 205, 127, 50, 100 + 20 * sin(currentTime/500.0f));
                }
                SDL_RenderFillRect(screen, &glowRect);
            }

            // Score box background
            SDL_SetRenderDrawColor(screen, 30, 30, 50, 200);
            SDL_RenderFillRect(screen, &scoreRect);

            // Score box border
            SDL_SetRenderDrawColor(screen, 150, 150, 150, 255);
            SDL_RenderDrawRect(screen, &scoreRect);

            // Center score text in box
            int scoreX = scoreRect.x + (scoreRect.w - score_item.GetWidth()) / 2;
            score_item.RenderText(screen, scoreX, startY);

            startY += 50;
        }

        // Navigation note with pulsing effect
        TextOb note;
        note.SetFont(noteFont);
        note.SetText("Press ESC to return to menu");
        int noteAlpha = 150 + 105 * sin(currentTime/500.0f);
        note.SetColor(200, 200, 220);
        note.LoadFromRenderText(noteFont, screen);
        note.RenderText(screen, SCREEN_WIDTH / 2 - note.GetWidth()/2, SCREEN_HEIGHT - 50);

        SDL_RenderPresent(screen);

        // Cap framerate
        SDL_Delay(16); // ~60 FPS
    }

    // Clean up resources
    TTF_CloseFont(titleFont);
    TTF_CloseFont(scoreFont);
    TTF_CloseFont(noteFont);
}


void MainMenu::Render(SDL_Renderer* screen) {
    background_.Render(screen, nullptr);

    // Hiển thị tiêu đề
    TextOb title_text;
    title_text.SetText("R.O.B.O: Ruined Factory");
    title_text.SetColor(TextOb::WHITE_TEXT);
    title_text.SetFont(font_);
    title_text.LoadFromRenderText(font_, screen);

    int title_x = SCREEN_WIDTH / 2 - title_text.GetWidth() / 2;
    int title_y = 80;

    // Khung tiêu đề
    SDL_Rect title_frame = {
        title_x - 20,
        title_y - 20,
        title_text.GetWidth() + 40,
        title_text.GetHeight() + 40
    };

    SDL_SetRenderDrawBlendMode(screen, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(screen, 0, 0, 100, 180); // Xanh đậm + trong suốt
    SDL_RenderFillRect(screen, &title_frame);

    SDL_SetRenderDrawColor(screen, 255, 255, 255, 255);
    SDL_RenderDrawRect(screen, &title_frame);

    title_text.RenderText(screen, title_x, title_y);

    // Render Options
    int option_start_y = title_y + 150;
    int space_between_options = 70;

    int max_width = 0;
    for (const auto& opt : options_) {
        TextOb temp;
        temp.SetText(opt);
        temp.SetFont(font_);
        temp.LoadFromRenderText(font_, screen);
        max_width = std::max(max_width, temp.GetWidth());
    }
    max_width += 40;

    for (size_t i = 0; i < options_.size(); ++i) {
        TextOb opt_text;
        opt_text.SetText(options_[i]);
        opt_text.SetFont(font_);
        opt_text.SetColor(i == selected_index_ ? TextOb::RED_TEXT : TextOb::WHITE_TEXT);
        opt_text.LoadFromRenderText(font_, screen);

        int opt_x = SCREEN_WIDTH / 2 - opt_text.GetWidth() / 2;
        int opt_y = option_start_y + i * space_between_options;

        SDL_Rect option_rect = {
            SCREEN_WIDTH / 2 - max_width / 2,
            opt_y - 10,
            max_width,
            opt_text.GetHeight() + 20
        };

        if (i == selected_index_) {
            SDL_SetRenderDrawColor(screen, 120, 20, 20, 200); // Nền đỏ nhạt nếu hover
        } else {
            SDL_SetRenderDrawColor(screen, 30, 30, 60, 150); // Nền tối
        }
        SDL_RenderFillRect(screen, &option_rect);

        SDL_SetRenderDrawColor(screen, 255, 255, 255, 255);
        SDL_RenderDrawRect(screen, &option_rect);

        opt_text.RenderText(screen, opt_x, opt_y);
    }

    // Nếu Game Over, vẽ cửa sổ Game Over
    if (game_over_ && game_over_texture_ != nullptr) {
        SDL_SetRenderDrawBlendMode(screen, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(screen, 0, 0, 0, 150); // Overlay mờ nền
        SDL_Rect full_screen = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderFillRect(screen, &full_screen);

        SDL_Rect destRect = {
            (SCREEN_WIDTH - GAME_OVER_WIDTH) / 2,
            (SCREEN_HEIGHT - GAME_OVER_HEIGHT) / 2,
            GAME_OVER_WIDTH,
            GAME_OVER_HEIGHT
        };
        SDL_RenderCopy(screen, game_over_texture_, NULL, &destRect);

        // Nút Exit
        SDL_Rect exit_btn = {
            destRect.x + 50,
            destRect.y + destRect.h - 80,
            120, 50
        };
        SDL_SetRenderDrawColor(screen, 200, 0, 0, 255);
        SDL_RenderFillRect(screen, &exit_btn);

        TextOb exit_text;
        exit_text.SetText("Exit");
        exit_text.SetFont(font_);
        exit_text.SetColor(TextOb::WHITE_TEXT);
        exit_text.LoadFromRenderText(font_, screen);
        exit_text.RenderText(screen, exit_btn.x + (exit_btn.w - exit_text.GetWidth()) / 2, exit_btn.y + (exit_btn.h - exit_text.GetHeight()) / 2);

        // Nút Continue
        SDL_Rect continue_btn = {
            destRect.x + destRect.w - 170,
            destRect.y + destRect.h - 80,
            120, 50
        };
        SDL_SetRenderDrawColor(screen, 0, 200, 0, 255);
        SDL_RenderFillRect(screen, &continue_btn);

        TextOb continue_text;
        continue_text.SetText("Continue");
        continue_text.SetFont(font_);
        continue_text.SetColor(TextOb::WHITE_TEXT);
        continue_text.LoadFromRenderText(font_, screen);
        continue_text.RenderText(screen, continue_btn.x + (continue_btn.w - continue_text.GetWidth()) / 2, continue_btn.y + (continue_btn.h - continue_text.GetHeight()) / 2);
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
