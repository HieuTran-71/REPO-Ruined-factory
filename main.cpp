#include "BaseObject.h"
#include "Game_map.h"
#include "MainOb.h"
#include "Timer.h"
#include "ThreatOb.h"
#include "ExplosionOb.h"
#include "TextOb.h"
#include "PlayPower.h"
#include "SoundManager.h"
#include "MainMenu.h"
#include <iostream>


BaseObject g_background;
TTF_Font* font_time;
SoundManager* g_sound_manager = nullptr;
// Biến toàn cục để lưu trạng thái background
SDL_Texture* g_current_background = nullptr;
SDL_Texture* g_game_background = nullptr; // Để lưu background của game
SDL_Texture* g_menu_background = nullptr; // Để lưu background của menu

bool InitData() {
    bool success = true;
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (ret < 0)
        return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    g_window = SDL_CreateWindow("R.O.B.O : Ruined factory", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (g_window == NULL) {
        success = false;
    }
    else {
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if (g_screen == NULL)
            success = false;
        else {
            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags))
                success = false;
        }

        if (TTF_Init() == -1) {
            success = false;
        }

        font_time = TTF_OpenFont("font/CyberpunkCraftpixPixel.otf", 20);
        if (font_time == NULL) {
            success = false;
        }

        g_sound_manager = SoundManager::GetInstance();
        if (!g_sound_manager->Init()) {
            return false;
        }

        g_sound_manager->LoadSound(SOUND_MUSIC, "Sound/background.mp3");
        g_sound_manager->LoadSound(SOUND_BULLET, "Sound/bullet.wav");
        g_sound_manager->LoadSound(SOUND_EXPLOSION, "Sound/explosion.wav");
        g_sound_manager->LoadSound(SOUND_JUMP, "Sound/jump.wav");
        g_sound_manager->LoadSound(SOUND_COLLECT, "Sound/collect.wav");
        g_sound_manager->LoadSound(SOUND_GAMEOVER, "Sound/game_over.wav");

    }

    return success;
}

bool LoadBackground() {
    return g_background.LoadImg("image/background.png", g_screen);
}

void close() {
    g_background.Free();


    if (font_time != NULL) {
        TTF_CloseFont(font_time);
        font_time = NULL;
    }


    if (g_sound_manager != nullptr) {
        g_sound_manager->Clean();
        g_sound_manager = nullptr;
    }


    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;
    SDL_DestroyWindow(g_window);
    g_window = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


std::vector<ThreatOb*> MakeThreatList(Map& map_data) {
    std::vector<ThreatOb*> list_threats;

    for (int y = 0; y < MAX_MAP_Y; ++y) {
        for (int x = 0; x < MAX_MAP_X; ++x) {
            int val = map_data.tile[y][x];

            if (val == 21) {
                ThreatOb* p_threat = new ThreatOb();

                p_threat->LoadImg("image/threat_Lv1_L .png", g_screen);
                p_threat->set_clips();
                p_threat->set_type_move(ThreatOb::MOVE_IN_SPACE);

                int pos_x = x * TILE_SIZE;
                int pos_y = y * TILE_SIZE;

                p_threat->set_x_pos(pos_x);
                p_threat->set_y_pos(pos_y);

                int pos1 = pos_x - 200;
                int pos2 = pos_x + 200;
                p_threat->SetAnimation(pos1, pos2);
                p_threat->set_input_left(1);

                list_threats.push_back(p_threat);


                map_data.tile[y][x] = 0;
            }
        }
    }

    return list_threats;
}

struct ExplosionInfo {
    int x, y;
    int current_frame;
    bool from_bullet;  // true = từ đạn, false = từ player
    ImpTimer timer;
};

std::vector<std::string> map_list = {
    "map/map01.dat",
    "map/map02.dat"

};

void ResetMap(GameMap& game_map, Map& map_data, std::vector<ThreatOb*>& list_threats, MainOb& p_player, SDL_Renderer* g_screen, const std::vector<std::string>& map_list)
{
    // Random chọn map
    int rand_index = rand() % map_list.size();
    std::string map_file = map_list[rand_index];

    // Load map
    game_map.LoadMap(map_file);
    map_data = game_map.getMap();

    // Load Tiles nếu cần
    game_map.LoadTiles(g_screen);

    p_player.SetMapXY(0, 0);
    p_player.SetPos(0, 0);
    p_player.set_comeback_time(0);

    // Xoá threat cũ
    for (int i = 0; i < list_threats.size(); ++i)
    {
        ThreatOb* p_threat = list_threats[i];
        if (p_threat)
        {
            delete p_threat;
            list_threats[i] = nullptr;
        }
    }
    list_threats.clear();

    // Tạo threat mới
    list_threats = MakeThreatList(map_data);
}


std::vector<ExplosionInfo> active_explosions;

GameOverResult ShowGameOverScreen(SDL_Renderer* renderer, MainMenu& menu, int& current_mark) {
    // Load hình ảnh Game Over (đảm bảo đường dẫn đúng)
    SDL_Surface* game_over_surface = IMG_Load("image/game_over_window.png");
    if (!game_over_surface) {
        SDL_Log("IMG_Load: %s\n", IMG_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lỗi", "Không thể tải ảnh Game Over!", g_window);
        return GAMEOVER_EXIT;
    }
    SDL_Texture* game_over_texture = SDL_CreateTextureFromSurface(renderer, game_over_surface);
    SDL_FreeSurface(game_over_surface);
    if (!game_over_texture) {
        SDL_Log("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lỗi", "Không thể tạo texture Game Over!", g_window);
        return GAMEOVER_EXIT;
    }

    // Load nút xanh (Continue/Menu) và nút đỏ (Exit)
    SDL_Surface* blue_button_surface = IMG_Load("image/Button-Continue.png");
    SDL_Surface* red_button_surface = IMG_Load("image/Button-Exit.png");
    if (!blue_button_surface || !red_button_surface) {
        SDL_Log("IMG_Load (button): %s\n", IMG_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lỗi", "Không thể tải ảnh nút!", g_window);
        SDL_DestroyTexture(game_over_texture);
        return GAMEOVER_EXIT;
    }
    SDL_Texture* blue_button_texture = SDL_CreateTextureFromSurface(renderer, blue_button_surface);
    SDL_Texture* red_button_texture = SDL_CreateTextureFromSurface(renderer, red_button_surface);
    SDL_FreeSurface(blue_button_surface);
    SDL_FreeSurface(red_button_surface);
    if (!blue_button_texture || !red_button_texture) {
        SDL_Log("SDL_CreateTextureFromSurface (button): %s\n", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lỗi", "Không thể tạo texture nút!", g_window);
        SDL_DestroyTexture(game_over_texture);
        return GAMEOVER_EXIT;
    }

    // Tạo các TextOb cho các nội dung
    TextOb game_over_text;
    game_over_text.SetColor(TextOb::WHITE_TEXT);
    game_over_text.SetText("Game Over");
    game_over_text.LoadFromRenderText(font_time, renderer);

    TextOb continue_text;
    continue_text.SetColor(TextOb::BLACK_TEXT);
    continue_text.SetText("Continue");
    continue_text.LoadFromRenderText(font_time, renderer);

    TextOb exit_text;
    exit_text.SetColor(TextOb::BLACK_TEXT);
    exit_text.SetText("Exit");
    exit_text.LoadFromRenderText(font_time, renderer);

    TextOb score_text;
    score_text.SetColor(TextOb::WHITE_TEXT);
    std::string score_str = "Score: " + std::to_string(current_mark);
    score_text.SetText(score_str);
    score_text.LoadFromRenderText(font_time, renderer);

    SDL_Rect game_over_rect;
    game_over_rect.w = SCREEN_WIDTH / 2;
    game_over_rect.h = SCREEN_HEIGHT / 2;
    game_over_rect.x = (SCREEN_WIDTH - game_over_rect.w) / 2;
    game_over_rect.y = (SCREEN_HEIGHT - game_over_rect.h) / 2;

    int button_width = 120;
    int button_height = 60;
    int button_spacing = 30;

    SDL_Rect red_button_rect;  // Nút đỏ (Exit)
    red_button_rect.w = button_width;
    red_button_rect.h = button_height;
    red_button_rect.x = (SCREEN_WIDTH / 2) - button_width - button_spacing;
    red_button_rect.y = SCREEN_HEIGHT * 3 / 4;

    SDL_Rect blue_button_rect;  // Nút xanh (Continue/Menu)
    blue_button_rect.w = button_width;
    blue_button_rect.h = button_height;
    blue_button_rect.x = (SCREEN_WIDTH / 2) + button_spacing;
    blue_button_rect.y = SCREEN_HEIGHT * 3 / 4;

    // Tính toán vị trí văn bản
    int game_over_text_x = (SCREEN_WIDTH - game_over_text.GetWidth()) / 2;
    int game_over_text_y = game_over_rect.y + 100; // Khoảng cách từ trên xuống

    int score_text_x = (SCREEN_WIDTH - score_text.GetWidth()) / 2;
    int score_text_y = game_over_text_y + game_over_text.GetHeight() + 20; // Dưới Game Over

    int continue_text_x = blue_button_rect.x + (blue_button_rect.w - continue_text.GetWidth()) / 2;
    int continue_text_y = blue_button_rect.y + (blue_button_rect.h - continue_text.GetHeight()) / 2;

    int exit_text_x = red_button_rect.x + (red_button_rect.w - exit_text.GetWidth()) / 2;
    int exit_text_y = red_button_rect.y + (red_button_rect.h - exit_text.GetHeight()) / 2;

    bool game_over_done = false;
    GameOverResult result = GAMEOVER_EXIT; // Mặc định là thoát game

    while (!game_over_done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game_over_done = true;
                result = GAMEOVER_EXIT;
                break;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);

                if (mouse_x >= blue_button_rect.x && mouse_x <= blue_button_rect.x + blue_button_rect.w &&
                    mouse_y >= blue_button_rect.y && mouse_y <= blue_button_rect.y + blue_button_rect.h) {
                    // Người chơi nhấn nút xanh (Quay về menu)
                    game_over_done = true;
                    menu.SaveHighScore(current_mark);
                    result = GAMEOVER_MENU;
                    break;
                }
                else if (mouse_x >= red_button_rect.x && mouse_x <= red_button_rect.x + red_button_rect.w &&
                    mouse_y >= red_button_rect.y && mouse_y <= red_button_rect.y + red_button_rect.h) {
                    // Người chơi nhấn nút đỏ (Thoát)
                    game_over_done = true;
                    result = GAMEOVER_EXIT;
                    break;
                }
            }
        }

        // Vẽ background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen
        SDL_RenderClear(renderer);

        // Vẽ các thành phần
        SDL_RenderCopy(renderer, game_over_texture, NULL, &game_over_rect);
        SDL_RenderCopy(renderer, blue_button_texture, NULL, &blue_button_rect);
        SDL_RenderCopy(renderer, red_button_texture, NULL, &red_button_rect);

        // Render text
        game_over_text.RenderText(renderer, game_over_text_x, game_over_text_y);
        score_text.RenderText(renderer, score_text_x, score_text_y);
        continue_text.RenderText(renderer, continue_text_x, continue_text_y);
        exit_text.RenderText(renderer, exit_text_x, exit_text_y);

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    // Giải phóng
    SDL_DestroyTexture(game_over_texture);
    SDL_DestroyTexture(blue_button_texture);
    SDL_DestroyTexture(red_button_texture);

    return result;
}

bool PlayGame(int &current_mark, MainMenu& menu, bool& in_menu)
{
    ImpTimer fps_timer;
    bool is_quit = false;

    // Init lại game mỗi lần Play
    GameMap game_map;

    Map map_data = game_map.getMap();

    MainOb p_player;
    p_player.LoadImg("image/move-R.png", g_screen);
    p_player.set_clips();

    std::vector<ThreatOb*> list_threats = MakeThreatList(map_data);

    ResetMap(game_map, map_data, list_threats, p_player, g_screen, map_list);

    ExplosionOb exp_player;
    exp_player.LoadImg("image/exp_bullet.png", g_screen);
    exp_player.set_clips();

    ExplosionOb exp_main;
    exp_main.LoadImg("image/exp_bullet.png", g_screen);
    exp_main.set_clips();

    ExplosionOb exp_bullet_tile;
    exp_bullet_tile.LoadImg("image/exp_bullet.png", g_screen);
    exp_bullet_tile.set_clips();

    TextOb time_game;
    time_game.SetColor(TextOb::WHITE_TEXT);

    TextOb mark_game;
    mark_game.SetColor(TextOb::WHITE_TEXT);

    g_sound_manager->PlayMusic(-1);

    UINT mark_value = 0;
    std::string strMark;

    while (!is_quit) {
        fps_timer.start();
        while (SDL_PollEvent(&g_event) != 0) {
            if (g_event.type == SDL_QUIT) {
                is_quit = true;
            }

            p_player.HandelInputAction(g_event, g_screen);

        }

        SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
        SDL_RenderClear(g_screen);

        SDL_Rect bg_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        g_background.Render(g_screen, &bg_rect); // nen luon duoc can chinh theo man hinh

        game_map.DrawMap(g_screen);

        p_player.HandleBullet(g_screen, map_data, g_screen, &exp_bullet_tile);
        p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
        p_player.Do_Player(map_data);
        p_player.Show(g_screen, map_data);



        SDL_Rect player_frame = p_player.GetRectFrame();
        SDL_Rect player_rect = { static_cast<int>(p_player.GetXPos()), static_cast<int>(p_player.GetYPos()), player_frame.w, player_frame.h };

        int start_x = player_rect.x / TILE_SIZE;
        int end_x = (player_rect.x + player_rect.w - 1) / TILE_SIZE;
        int start_y = player_rect.y / TILE_SIZE;
        int end_y = (player_rect.y + player_rect.h - 1) / TILE_SIZE;

        // Giới hạn trong bản đồ
        start_x = std::max(0, start_x);
        end_x = std::min(MAX_MAP_X - 1, end_x);
        start_y = std::max(0, start_y);
        end_y = std::min(MAX_MAP_Y - 1, end_y);

        for (int y = start_y; y <= end_y; y++)
        {
            for (int x = start_x; x <= end_x; x++)
            {
                int tile_value = map_data.tile[y][x];

                if (tile_value == TILE_GATE)
                {
                    SDL_Rect gate_rect;
                    gate_rect.x = x * TILE_SIZE - map_data.start_x_;
                    gate_rect.y = y * TILE_SIZE - map_data.start_y_;
                    gate_rect.w = TILE_SIZE;
                    gate_rect.h = TILE_SIZE;

                    if (SDLCommonFunc::CheckCollision(player_rect, gate_rect))
                    {
                        ResetMap(game_map, map_data, list_threats, p_player, g_screen, map_list);
                    }
                }
            }
        }

        next_frame:; // Label để nhảy tới sau khi chuyển map



        // Kiểm tra nếu nhân vật rơi xuống vực
        if (p_player.GetRect().y > map_data.max_y_) {
            ExplosionInfo exp;
            exp.x = p_player.GetRect().x - exp_main.get_frame_width() * 0.07;
            exp.y = p_player.GetRect().y - exp_main.get_frame_height() * 0.25;
            exp.current_frame = 0;
            exp.from_bullet = false;
            exp.timer.start();
            active_explosions.push_back(exp);

            g_sound_manager->StopMusic();
            g_sound_manager->PlaySoundA(SOUND_EXPLOSION);

            // Hiển thị hiệu ứng nổ
            for (int i = 0; i < 20; i++) { // Hiển thị khoảng 20 frame
                SDL_RenderClear(g_screen);

                // Vẽ màn hình hiện tại
                SDL_Rect bg_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                g_background.Render(g_screen, &bg_rect);
                game_map.DrawMap(g_screen);
                p_player.Show(g_screen, map_data);
                for (int j = 0; j < list_threats.size(); j++) {
                    list_threats.at(j)->Show(g_screen);
                }
                // Hiển thị hiệu ứng nổ
                int frame_idx = i / 2; // 2 frames thực tế = 1 frame hiệu ứng
                if (frame_idx < NUM_FRAME_EXP) {
                    exp_main.set_frame(frame_idx);
                    exp_main.SetRect(exp.x, exp.y);
                    exp_main.Show(g_screen);
                }

                SDL_RenderPresent(g_screen);
                SDL_Delay(40); // 40ms mỗi frame
            }

            g_sound_manager->PlaySoundA(SOUND_GAMEOVER);

            SDL_Delay(1000);

            current_mark = mark_value;
            GameOverResult result = ShowGameOverScreen(g_screen, menu, current_mark);
            if (result == GAMEOVER_MENU)
            {
            // Quay về menu
                in_menu = true;
                return true;; // Thoát vòng lặp game hiện tại
            } else
            {
                // Thoát game
                is_quit = true;
                return false;
            }
            continue;
        }

        game_map.SetMap(map_data);
        game_map.DrawMap(g_screen);


        for (int i = 0; i < list_threats.size(); i++) {
            ThreatOb* p_threat = list_threats.at(i);
            if (p_threat != NULL) {
                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->ImpMoveType(g_screen);
                p_threat->DoPlayer(map_data);
                p_threat->Show(g_screen);

                SDL_Rect rect_player = p_player.GetRectFrame();
                SDL_Rect rect_threat = p_threat->GetRectFrame();
                bool player_threat_Col = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
                if (player_threat_Col) {
                    // Tạo hiệu ứng nổ mới
                    ExplosionInfo exp;
                    exp.x = p_player.GetRect().x - exp_main.get_frame_width() * 0.07;
                    exp.y = p_player.GetRect().y - exp_main.get_frame_height() * 0.25;
                    exp.current_frame = 0;
                    exp.from_bullet = false;
                    exp.timer.start();

                    g_sound_manager->StopMusic();
                    g_sound_manager->PlaySoundA(SOUND_EXPLOSION);

                    // Tạo một vòng lặp để chỉ hiển thị hiệu ứng nổ
                    bool explosion_done = false;
                    while (!explosion_done) {

                        // Hiển thị hiệu ứng nổ
                        int frame_duration = 40; // 40ms mỗi frame
                        int ticks = exp.timer.get_ticks();
                        int frame_idx = ticks / frame_duration;

                        if (frame_idx >= NUM_FRAME_EXP) {
                            explosion_done = true; // Kết thúc hiệu ứng nổ
                        }
                        else {
                            exp_main.set_frame(frame_idx);
                            exp_main.SetRect(exp.x, exp.y);
                            exp_main.Show(g_screen);
                        }

                        SDL_RenderPresent(g_screen);
                        SDL_Delay(20); // Giữ tốc độ ổn định
                    }

                    // Phát nhạc game over sau khi hiệu ứng nổ hoàn thành
                    g_sound_manager->PlaySoundA(SOUND_GAMEOVER);

                    SDL_Delay(1000);

                    current_mark = mark_value;
                    GameOverResult result = ShowGameOverScreen(g_screen, menu, current_mark);
                    if (result == GAMEOVER_MENU)
                    {
                        active_explosions.clear();
                        // Quay về menu
                        in_menu = true;
                        return true; // Thoát vòng lặp game hiện tại
                    } else
                    {
                        // Thoát game
                        is_quit = true;
                        return false;
                    }
                    continue;
                }
            }
        }


        int frame_exp_width = exp_player.get_frame_width();
        int frame_exp_height = exp_player.get_frame_height();
        std::vector<Bullet*> bullet_arr = p_player.get_bullet_list();
        for (int m = 0 ; m < bullet_arr.size(); m++)
        {
            Bullet* p_bullet = bullet_arr.at(m);
            if (p_bullet != NULL)
            {
                SDL_Rect bRect = p_bullet->GetRect();

                // Tọa độ thật của viên đạn trên bản đồ (chưa trừ offset khi render)
                int real_x = bRect.x + map_data.start_x_;
                int real_y = bRect.y + map_data.start_y_;

                // Lấy phạm vi tile xung quanh viên đạn
                int start_x = real_x / TILE_SIZE;
                int end_x = (real_x + bRect.w - 1) / TILE_SIZE;
                int start_y = real_y / TILE_SIZE;
                int end_y = (real_y + bRect.h - 1) / TILE_SIZE;

                // Giới hạn trong bản đồ
                start_x = std::max(0, start_x);
                end_x = std::min(MAX_MAP_X - 1, end_x);
                start_y = std::max(0, start_y);
                end_y = std::min(MAX_MAP_Y - 1, end_y);

                bool hit_tile = false;

                for (int y = start_y; y <= end_y; y++)
                {
                    for (int x = start_x; x <= end_x; x++)
                    {
                        int tile_value = map_data.tile[y][x];

                        if (tile_value > 0 && tile_value < MAX_TILES)
                        {
                            int tile_type = tile_type_mapping[tile_value];

                            if (tile_type == TILE_TYPE_SOLID)
                            {
                                SDL_Rect tile_rect;
                                tile_rect.x = x * TILE_SIZE - map_data.start_x_;
                                tile_rect.y = y * TILE_SIZE - map_data.start_y_;
                                tile_rect.w = TILE_SIZE;
                                tile_rect.h = TILE_SIZE;

                                if (SDLCommonFunc::CheckCollision(bRect, tile_rect))
                                {
                                    ExplosionInfo exp;
                                    exp.x = bRect.x - frame_exp_width * 0.07;
                                    exp.y = bRect.y - frame_exp_height * 0.25;
                                    exp.current_frame = 0;
                                    exp.from_bullet = true;
                                    exp.timer.start();
                                    active_explosions.push_back(exp);
                                    g_sound_manager->StopMusic();
                                    g_sound_manager->PlaySoundA(SOUND_EXPLOSION);



                                    p_bullet->set_is_move(false);
                                    hit_tile = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (hit_tile) break;
                }



                for (int t = 0 ; t < list_threats.size(); t++)
                {
                    ThreatOb* obj_threat = list_threats.at(t);
                    if (obj_threat != NULL)
                    {
                        SDL_Rect tRect = obj_threat->GetRect();

                        bool bCol1 = SDLCommonFunc::CheckCollision(bRect, tRect);

                        if (bCol1)
                        {
                            mark_value++;
                            ExplosionInfo exp;
                            exp.x = p_bullet->GetRect().x - frame_exp_width * 0.07;
                            exp.y = p_bullet->GetRect().y - frame_exp_height * 0.25;
                            exp.current_frame = 0;
                            exp.from_bullet = true;
                            exp.timer.start();
                            active_explosions.push_back(exp);
                            g_sound_manager->StopMusic();
                            g_sound_manager->PlaySoundA(SOUND_EXPLOSION);


                            p_player.RemoveBullet(m);
                            obj_threat->Free();
                            list_threats.erase(list_threats.begin() + t);
                            break;
                        }
                    }
                }
            }
        }

        std::string str_time = "Time: ";
        Uint32 time_val = SDL_GetTicks() / 1000;
        Uint32 val_time = 0 + time_val;
        if (val_time > 0)
        {
            std::string str_val = std::to_string(val_time);
            str_time += str_val;

            time_game.SetText(str_time);
            time_game.LoadFromRenderText(font_time, g_screen);
            time_game.RenderText(g_screen, SCREEN_WIDTH - 200, 15);
        }

        std::string val_str_mark = std::to_string(mark_value);
        std::string strMark("Mark: ");
        strMark += val_str_mark;

        mark_game.SetText(strMark);
        mark_game.LoadFromRenderText(font_time, g_screen);
        mark_game.RenderText(g_screen, SCREEN_WIDTH*0.5 - 50, 15);

        for (int i = 0; i < active_explosions.size(); )
        {
            ExplosionInfo& exp = active_explosions[i];
            int frame_duration = 40; // 40ms mỗi frame
            int ticks = exp.timer.get_ticks();
            int frame_idx = ticks / frame_duration;

            if (frame_idx >= NUM_FRAME_EXP)
            {
                active_explosions.erase(active_explosions.begin() + i);
            }
            else
            {
                if (exp.from_bullet)
                {
                    exp_bullet_tile.set_frame(frame_idx);
                    exp_bullet_tile.SetRect(exp.x, exp.y);
                    exp_bullet_tile.Show(g_screen);
                }
                else
                {
                    exp_main.set_frame(frame_idx);
                    exp_main.SetRect(exp.x, exp.y);
                    exp_main.Show(g_screen);
                }

                ++i;
            }
        }



        SDL_RenderPresent(g_screen);

        int real_imp_time = fps_timer.get_ticks(); // thoi gian thuc su da troi qua
        int time_one_frame = 1000/FRAME_PER_SECOND; // ms

        if (real_imp_time < time_one_frame)
        {
            int delay_time = time_one_frame - real_imp_time; // tao do tre de can bang
            if (delay_time >= 0)
                SDL_Delay(delay_time);
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (InitData() == false) {
        return -1;
    }

    // Load background game
    SDL_Surface* game_bg_surface = IMG_Load("image/background.png");
    if (!game_bg_surface) {
        SDL_Log("IMG_Load: %s\n", IMG_GetError());
        return -1;
    }
    g_game_background = SDL_CreateTextureFromSurface(g_screen, game_bg_surface);
    SDL_FreeSurface(game_bg_surface);

    if (!g_game_background) {
        SDL_Log("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        return -1;
    }

    // Load background menu
    SDL_Surface* menu_bg_surface = IMG_Load("image/menu_background.png");
    if (!menu_bg_surface) {
        SDL_Log("IMG_Load: %s\n", IMG_GetError());
        return -1;
    }
    g_menu_background = SDL_CreateTextureFromSurface(g_screen, menu_bg_surface);
    SDL_FreeSurface(menu_bg_surface);

    if (!g_menu_background) {
        SDL_Log("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        return -1;
    }

    g_current_background = g_menu_background; // Khởi tạo background là menu

    if (LoadBackground() == false) {
        return -1;
    }

    srand(time(NULL));

    MainMenu menu;
    if (!menu.LoadMenu(g_screen)) {
        return -1;
    }

    bool quit_program = false;

    while (!quit_program)
    {
        bool in_menu = true;
        int current_mark = 0;

        // MENU LOOP
        while (in_menu)
        {
            while (SDL_PollEvent(&g_event)) {
                if (g_event.type == SDL_QUIT) {
                    quit_program = true;
                    in_menu = false;
                    break;
                }

                int option = menu.HandleEvent(g_event, current_mark);
                if (option == MainMenu::MENU_PLAY) {
                    in_menu = false; // Vào game
                }
                else if (option == MainMenu::MENU_HIGHSCORE) {
                    menu.ShowHighScoreBoard(g_screen);
                }
                else if (option == MainMenu::MENU_EXIT) {
                    quit_program = true;
                    in_menu = false;
                    break;
                }
            }

            SDL_RenderClear(g_screen);
            menu.Render(g_screen);
            SDL_RenderPresent(g_screen);
        }

        if (quit_program) {
            break;
        }

        bool game_running = PlayGame(current_mark, menu, in_menu);

        if (!game_running) {
            quit_program = true;
            break;
        }
    }

    close();
    return 0;
}
