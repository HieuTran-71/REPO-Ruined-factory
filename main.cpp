
#include "BaseObject.h"
#include "Game_map.h"
#include "MainOb.h"
#include "Timer.h"
#include "ThreatOb.h"

BaseObject g_background;

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
            p_threat->LoadImg("image/threat_Lv1_L.png", g_screen);
            p_threat->set_clips();
            p_threat->set_type_move(ThreatOb::MOVE_IN_SPACE);
            p_threat->set_x_pos(500 + i*500);
            p_threat->set_y_pos(200);

            int pos1 = p_threat->get_x_pos() - 60;
            int pos2 = p_threat->get_x_pos() + 60;
            p_threat->SetAnimation(pos1,pos2);
            p_threat->set_input_left(1);
            list_threats.push_back(p_threat);
        }
    }

    ThreatOb* threats_objs = new ThreatOb[20];

    for (int i = 0 ;i < 20; i++)
    {
        ThreatOb* p_threat = (threats_objs + i);
        if (p_threat != NULL)
        {
            p_threat->LoadImg("image/threat_Lv1_R.png", g_screen);
            p_threat->set_clips();
            p_threat->set_x_pos(700 + i*1200);
            p_threat->set_y_pos(250);

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


    MainOb p_player;
    p_player.LoadImg("image/move-R.png", g_screen);
    p_player.set_clips();

    std::vector<ThreatOb*> list_threats = MakeThreatList();


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

        Map map_data = game_map.getMap();

        p_player.HandleBullet(g_screen);
        p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
        p_player.Do_Player(map_data);
        p_player.Show(g_screen, map_data);

        game_map.SetMap(map_data);
        game_map.DrawMap(g_screen);

        for (int i = 0 ; i < list_threats.size(); i++)
        {
            ThreatOb* p_threat = list_threats.at(i);
            if (p_threat != NULL)
            {
                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->ImpMoveType(g_screen);
                p_threat->DoPlayer(map_data);
                p_threat->Show(g_screen);
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

    close();
    return 0;
}
