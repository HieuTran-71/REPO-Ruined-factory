#include "MainMenu.h"
#include "TextOb.h"

MainMenu::MainMenu() : selected_index_(0), font_(nullptr) {}

MainMenu::~MainMenu() {
    if (font_ != nullptr) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }
}

bool MainMenu::LoadMenu(SDL_Renderer* screen) {
    // Nạp ảnh nền
    background_.LoadImg("image/menu_background.png", screen);

    // Nạp ảnh panel menu
    menu_panel_.LoadImg("image/menu_panel.png", screen);

    // Nạp font
    font_ = TTF_OpenFont("font/SFUFuturaBook.TTF", 28);
    options_ = {"Play Game", "Exit"};

    return (font_ != nullptr);
}

void MainMenu::Render(SDL_Renderer* screen) {
    // Vẽ nền
    background_.Render(screen, nullptr);

    SDL_Rect panel_pos = {
        SCREEN_WIDTH / 2 - menu_panel_.GetRect().w / 2,  // x ở giữa màn hình
        100,                                             // y là 100px từ trên xuống
        0,                                               // w và h sẽ lấy theo kích thước texture
        0
    };

    // Vẽ panel menu với vị trí đã xác định
    menu_panel_.Render(screen, &panel_pos);
    // Vẽ tiêu đề game trong panel (nếu cần)
    TextOb title_text;
    title_text.SetText("GAME TITLE");
    title_text.SetColor(TextOb::WHITE_TEXT);
    title_text.LoadFromRenderText(font_, screen);
    title_text.RenderText(screen, SCREEN_WIDTH / 2 - title_text.GetWidth() / 2, 130);

    // Vẽ các tùy chọn menu
    for (int i = 0; i < options_.size(); ++i) {
        TextOb text;
        text.SetText(options_[i]);
        text.SetColor(i == selected_index_ ? TextOb::RED_TEXT : TextOb::WHITE_TEXT);
        text.LoadFromRenderText(font_, screen);
        text.RenderText(screen, SCREEN_WIDTH / 2 - text.GetWidth() / 2, 220 + i * 50);
    }
}

int MainMenu::HandleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mouse_x = event.button.x;
        int mouse_y = event.button.y;

        for (size_t i = 0; i < options_.size(); ++i) {
            int text_width = 0;
            int text_height = 0;
            TTF_SizeText(font_, options_[i].c_str(), &text_width, &text_height);

            // Sửa lại vùng phát hiện click chuột
            SDL_Rect rect = {
                SCREEN_WIDTH / 2 - text_width / 2,
                220 + static_cast<int>(i * 50),  // Điều chỉnh y để phù hợp với vị trí hiển thị mới
                text_width,
                text_height
            };

            if (mouse_x >= rect.x && mouse_x <= rect.x + rect.w &&
                mouse_y >= rect.y && mouse_y <= rect.y + rect.h) {
                return (i == 0 ? MENU_PLAY : MENU_EXIT);
            }
        }
    }

    // Xử lý sự kiện di chuyển chuột để highlight menu item
    if (event.type == SDL_MOUSEMOTION) {
        int mouse_x = event.motion.x;
        int mouse_y = event.motion.y;

        for (size_t i = 0; i < options_.size(); ++i) {
            int text_width = 0;
            int text_height = 0;
            TTF_SizeText(font_, options_[i].c_str(), &text_width, &text_height);

            SDL_Rect rect = {
                SCREEN_WIDTH / 2 - text_width / 2,
                220 + static_cast<int>(i * 50),  // Điều chỉnh y để phù hợp với vị trí hiển thị mới
                text_width,
                text_height
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
                return (selected_index_ == 0 ? MENU_PLAY : MENU_EXIT);
            default:
                break;
        }
    }

    return MENU_NONE;
}
