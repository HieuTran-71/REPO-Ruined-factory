#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include "CommonFunc.h"
#include "BaseObject.h"

class MainMenu {
public:
    MainMenu();
    ~MainMenu();
    bool LoadMenu(SDL_Renderer* screen);
    void Render(SDL_Renderer* screen);
    int HandleEvent(SDL_Event& event); // Trả về lựa chọn

    enum MenuOption {
        MENU_NONE,
        MENU_PLAY,
        MENU_EXIT
    };

private:
    BaseObject background_;
    BaseObject menu_panel_;  // Thêm đối tượng cho tấm ảnh panel
    std::vector<std::string> options_;
    int selected_index_;
    TTF_Font* font_;
};

#endif
