
#include "BaseObject.h"
#include "Game_map.h"
#include "MainOb.h"
#include "Timer.h"
#include "ThreatOb.h"
#include "ExplosionOb.h"
#include "TextOb.h"
#include "PlayPower.h"
#include "LazerBoss.h"

BaseObject g_background;
TTF_Font* font_time;
LaserBeam g_laserBeam;

bool InitData()
{
    bool success = true;
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if(ret < 0)
        return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");

    g_window = SDL_CreateWindow("R.O.B.O : Ruined factory", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if(g_window == NULL)
    {
        success = false;
    }
    else
    {
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if(g_screen == NULL)
            success = false;
        else
        {
            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            int imgFlags = IMG_INIT_PNG;
            if(!(IMG_Init(imgFlags) && imgFlags))
                success = false;
        }

        if (TTF_Init()== -1)
        {
            success = false;
        }

        font_time = TTF_OpenFont("font/SFUFuturaBook.TTF", 20);
        if (font_time == NULL)
        {
            success = false;
        }
        if (!g_laserBeam.LoadImg("image/laser_beam.png", g_screen))
        {
            success = false;
        }
    }

    return success;
}

bool LoadBackground()
{
    bool ret = g_background.LoadImg("image/background.png", g_screen);
    if(ret == false)
        return false;

    return true;
}

void close()
{
    g_background.Free();
    g_laserBeam.Free();

    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;

    SDL_DestroyWindow(g_window);
    g_window = NULL;

    IMG_Quit();
    SDL_Quit();
}


std::vector<ThreatOb*> MakeThreatList()
{
    std::vector<ThreatOb*> list_threats;

    ThreatOb* dynamic_threats = new ThreatOb[20];
    for (int i = 0 ; i < 20 ; i++)
    {
        ThreatOb* p_threat = dynamic_threats + i;
        if (p_threat != NULL)
        {
            p_threat->LoadImg("image/threat_Lv1_L .png", g_screen);
            p_threat->set_clips();
            p_threat->set_type_move(ThreatOb::MOVE_IN_SPACE);
            p_threat->set_x_pos(1000 + i*1000);
            p_threat->set_y_pos(200);

            int pos1 = p_threat->get_x_pos() - 200;
            int pos2 = p_threat->get_x_pos() + 200;
            p_threat->SetAnimation(pos1,pos2);
            p_threat->set_input_left(1);
            list_threats.push_back(p_threat);
        }
    }

    return list_threats;
}


int main(int argc, char* argv[])
{
    ImpTimer fps_timer;

    if (InitData() == false) {
        return -1;
    }

    if (LoadBackground() == false) {
        return -1;
    }

    GameMap game_map;
    game_map.LoadMap("map/map01.dat");
    game_map.LoadTiles(g_screen);

    Map map_data = game_map.getMap();

    MainOb p_player;
    p_player.LoadImg("image/move-R.png", g_screen);
    p_player.set_clips();

    PlayMoney player_money;
    player_money.Init(g_screen);
    player_money.SetPos(SCREEN_WIDTH*0.5 - 300, 8);

    std::vector<ThreatOb*> list_threats = MakeThreatList();

    ExplosionOb exp_player;
    bool tRet = exp_player.LoadImg("image/exp_bullet.png", g_screen);
    if (!tRet) return -1;
    exp_player.set_clips();

    ExplosionOb exp_main;
    bool mRet = exp_main.LoadImg("image/exp_bullet.png",g_screen);
    if (!mRet) return -1;
    exp_main.set_clips();

    TextOb time_game;
    time_game.SetColor(TextOb::WHITE_TEXT);

    TextOb mark_game;
    mark_game.SetColor(TextOb::WHITE_TEXT);
    UINT mark_value = 0;

    g_laserBeam.set_x_pos(0); // Bắt đầu từ bên trái màn hình
    g_laserBeam.set_y_pos(0); // Trên cùng
    g_laserBeam.set_is_active(true);

    bool is_laser_active = false;
    int player_moved_tiles = 0;
    int initial_player_x = 0;

    bool is_quit = false;
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

        SDL_Rect bg_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        g_background.Render(g_screen, &bg_rect); // nen luon duoc can chinh theo man hinh


        game_map.DrawMap(g_screen);

        p_player.HandleBullet(g_screen);
        p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
        p_player.Do_Player(map_data);
        p_player.Show(g_screen, map_data);

        game_map.SetMap(map_data);
        game_map.DrawMap(g_screen);

        player_money.Show(g_screen);

        // Kiểm tra số ô đã di chuyển để kích hoạt laser
        if (!is_laser_active) {
            if (initial_player_x == 0) {
                initial_player_x = p_player.GetRect().x / TILE_SIZE;
            }
            int current_player_x_tiles = p_player.GetRect().x / TILE_SIZE;
            player_moved_tiles = std::abs(current_player_x_tiles - initial_player_x);
            if (player_moved_tiles >= 5) {
                is_laser_active = true;
            }
        }


        if (is_laser_active) {

            g_laserBeam.Update(p_player);
            // Tạo vùng để scale laser cao bằng màn hình

            SDL_Rect laser_rect;
            laser_rect.x = g_laserBeam.get_x_pos();
            laser_rect.y = 0;
            laser_rect.w = g_laserBeam.get_width();
            laser_rect.h = SCREEN_HEIGHT;

            // Vẽ laser
            g_laserBeam.Render(g_screen, &laser_rect);

            // Kiểm tra va chạm với người chơi
            SDL_Rect player_rect = p_player.GetRectFrame();
            if (SDLCommonFunc::CheckCollision(laser_rect, player_rect)) {
                // Hiệu ứng nổ
                for (int ex = 0 ; ex < NUM_FRAME_EXP; ex++) {
                    int x_pos = p_player.GetRect().x - exp_main.get_frame_width() * 0.07;
                    int y_pos = p_player.GetRect().y - exp_main.get_frame_height() * 0.25;

                    exp_main.set_frame(ex);
                    exp_main.SetRect(x_pos, y_pos);
                    exp_main.Show(g_screen);
                    SDL_RenderPresent(g_screen);
                    SDL_Delay(50);
                }

                if (MessageBoxW(NULL, L"Bạn bị thiêu bởi laser đỏ!", L"Game Over", MB_OK | MB_ICONSTOP) == IDOK) {
                    close();
                    SDL_Quit();
                    return 0;
                }
            }
        }

        for (int i = 0 ; i < list_threats.size(); i++)
        {
            ThreatOb* p_threat = list_threats.at(i);
            if (p_threat != NULL)
            {
                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->ImpMoveType(g_screen);
                p_threat->DoPlayer(map_data);
                p_threat->Show(g_screen);

                SDL_Rect rect_player = p_player.GetRectFrame();
                SDL_Rect rect_threat = p_threat->GetRectFrame();
                bool player_threat_Col = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
                if (player_threat_Col)
                {
                    for (int ex = 0 ; ex < NUM_FRAME_EXP; ex++)
                    {
                        int x_pos = p_player.GetRect().x - exp_main.get_frame_width() * 0.07;
                        int y_pos = p_player.GetRect().y - exp_main.get_frame_height() * 0.25;

                        exp_main.set_frame(ex);
                        exp_main.SetRect(x_pos, y_pos);
                        exp_main.Show(g_screen);
                        SDL_RenderPresent(g_screen);
                        SDL_Delay(50);
                    }

                    if (MessageBoxW(NULL, L"Game Over", L"Info", MB_OK | MB_ICONSTOP) == IDOK)
                    {
                        p_threat->Free();
                        close();
                        SDL_Quit();
                        return 0;
                    }
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
                for (int t = 0 ; t < list_threats.size(); t++)
                {
                    ThreatOb* obj_threat = list_threats.at(t);
                     if (obj_threat != NULL)
                    {
                        SDL_Rect tRect;
                        tRect.x = obj_threat->GetRect().x;
                        tRect.y = obj_threat->GetRect().y;
                        tRect.w = obj_threat->get_width_frame();
                        tRect.h = obj_threat->get_height_frame();

                        SDL_Rect bRect = p_bullet->GetRect();

                        bool bCol1 = SDLCommonFunc::CheckCollision(bRect, tRect);

                        if (bCol1)
                        {
                            mark_value++;
                            for (int ex = 0; ex < NUM_FRAME_EXP; ex++)
                            {
                                int x_pos = p_bullet->GetRect().x - frame_exp_width*0.07;
                                int y_pos = p_bullet->GetRect().y - frame_exp_height*0.25;

                                exp_player.set_frame(ex);
                                exp_player.SetRect(x_pos, y_pos);
                                exp_player.Show(g_screen);
                                SDL_RenderPresent(g_screen);
                                SDL_Delay(50);


                            }

                            p_player.RemoveBullet(m);
                            obj_threat->Free();
                            list_threats.erase(list_threats.begin() + t);
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

    for (int i = 0; i < list_threats.size(); i++)
    {
        ThreatOb* p_threat = list_threats.at(i);
        if (p_threat)
        {
            p_threat->Free();
            p_threat = NULL;
        }
    }

    list_threats.clear();

    close();
    return 0;
}
