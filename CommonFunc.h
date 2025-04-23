
#ifndef COMMON_FUNCTION_H_
#define COMMON_FUNCTION_H_

#include <windows.h>
#include <cstring>
#include <string>
#include <cmath>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_screen = NULL;
static SDL_Event g_event;

const int FRAME_PER_SECOND = 37; // fps
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 640;
const int SCREEN_BPP = 32;

const int COLOR_KEY_R = 167;
const int COLOR_KEY_G = 175;
const int COLOR_KEY_B = 180;

const int RENDER_DRAW_COLOR = 0xff;

#define TILE_SIZE 64

#define MAX_MAP_X 400
#define MAX_MAP_Y 10

#define STATE_SUPPORT 4

typedef struct Input
{
    int left_;
    int right_;
    int up_;
    int down_;
    int jump_;
};


typedef struct Map
{
    int start_x_;
    int start_y_;

    int max_x_;
    int max_y_;

    int tile[MAX_MAP_Y][MAX_MAP_X];
    char* file_name;
};


enum TileType
{
    TILE_TYPE_BLANK = 0,
    TILE_TYPE_SOLID = 1,
    TILE_TYPE_WATER = 2,
    TILE_TYPE_SPIKE = 3,

};

const int MAX_TILES = 10;

static const int tile_type_mapping[MAX_TILES] =
{
    TILE_TYPE_BLANK, //0
    TILE_TYPE_SOLID, //1
    TILE_TYPE_SOLID, //2
    TILE_TYPE_SOLID, //3
    TILE_TYPE_BLANK, //4
    TILE_TYPE_SOLID, //5
    TILE_TYPE_SOLID, //6
    TILE_TYPE_SOLID, //7
    TILE_TYPE_SOLID, //8
    TILE_TYPE_SOLID, //9
};


namespace SDLCommonFunc
{
    bool CheckCollision(const SDL_Rect& object1, const SDL_Rect& object2);

}

#endif
