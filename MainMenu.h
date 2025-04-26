#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include "CommonFunc.h"
#include "BaseObject.h"
#include "TextOb.h"
#include "CommonFunc.h"


enum GameOverResult
{
    GAMEOVER_EXIT,
    GAMEOVER_MENU
};

class MainMenu {
public:
    MainMenu();
    ~MainMenu();

    // Core menu functions
    bool LoadMenu(SDL_Renderer* screen);
    void Render(SDL_Renderer* screen);
    int HandleEvent(SDL_Event& event, int& current_mark);

    // High score management
    void LoadHighScore();
    void SaveHighScore(int current_mark);
    void ShowHighScoreBoard(SDL_Renderer* screen);
    int GetHighScore() const { return high_score_; }
    void ClearHighScores(); // New function to reset all high scores
    bool IsNewHighScore(int score) const; // Check if a score would be a new high score

    // Game over screen
    void SetGameOver(bool game_over);
    void SetLastScore(int score) { last_score_ = score; }

    // Settings
    void ToggleFullscreen(SDL_Renderer* screen);
    bool IsFullscreen() const { return fullscreen_; }

    enum MenuOption {
        MENU_NONE,
        MENU_PLAY,
        MENU_HIGHSCORE,
        MENU_SETTINGS,
        MENU_CREDITS,
        MENU_EXIT
    };

private:
    // Helper functions
    void RenderMenuBackground(SDL_Renderer* screen);
    void RenderMenuOptions(SDL_Renderer* screen);
    void RenderSettingsMenu(SDL_Renderer* screen);
    void RenderCreditsScreen(SDL_Renderer* screen);

    // UI Elements
    BaseObject background_;
    BaseObject logo_;
    BaseObject menuFrame_;
    std::vector<BaseObject> decorations_; // For additional UI elements

    // Menu state
    std::vector<std::string> options_;
    int selected_index_;
    bool in_settings_menu_;
    bool in_credits_screen_;
    MenuOption current_menu_state_;

    // Scores
    std::vector<int> high_scores_;
    int high_score_;
    int last_score_;

    // Fonts and text
    TTF_Font* font_;
    TextOb highScoreText_;

    // Visual settings
    bool fullscreen_;

    // Game over
    bool game_over_;
    SDL_Texture* game_over_texture_;
    SDL_Renderer* g_screen;
};

// Global functions
GameOverResult ShowGameOverScreen(SDL_Renderer* renderer, MainMenu& menu, int& current_mark);
void ShowCreditsScreen(SDL_Renderer* renderer);
void ShowSettingsScreen(SDL_Renderer* renderer, MainMenu& menu);

#endif
