#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include "CommonFunc.h"
#include "BaseObject.h"
#include "TextOb.h"

class MainMenu {
public:
    MainMenu();
    ~MainMenu();
    bool LoadMenu(SDL_Renderer* screen);
    void Render(SDL_Renderer* screen);
    int HandleEvent(SDL_Event& event, int& current_mark); // Nhận điểm hiện tại
    void LoadHighScore();
    void SaveHighScore(int current_mark);
    int GetHighScore() const { return high_score_; }
    void ShowHighScoreBoard(SDL_Renderer* screen);
    void SetGameOver(bool game_over);

    enum MenuOption {
        MENU_NONE,
        MENU_PLAY,
        MENU_HIGHSCORE,
        MENU_EXIT
    };

    enum GameOverResult {
        GAMEOVER_EXIT,
        GAMEOVER_MENU
    };

private:
    BaseObject background_;
    std::vector<std::string> options_;
    std::vector<int> high_scores_;
    int selected_index_;
    TTF_Font* font_;
    int high_score_;
    TextOb highScoreText_;
    bool game_over_;
    SDL_Texture* game_over_texture_;
    SDL_Renderer* g_screen;
};


GameOverResult ShowGameOverScreen(SDL_Renderer* renderer, MainMenu& menu, int& current_mark);

#endif
