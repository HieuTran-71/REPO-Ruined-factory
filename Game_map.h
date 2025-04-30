#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "CommonFunc.h"
#include "BaseObject.h"

#define MAX_TILES 20
#define TILE_GATE 12

class TileMat : public BaseObject
{
public:
    TileMat(){;}
    ~TileMat() {;}
}; // hinh anh tinh duoc fill vao map

class GameMap
{
public:
    GameMap(){;}
    ~GameMap(){;}

    void LoadMap(const std::string& name);
    void LoadTiles(SDL_Renderer* Screen);
    void DrawMap(SDL_Renderer* screen);
    Map getMap() const {return game_map_;}; // lay ra map
    void SetMap(Map& map_data) {game_map_ = map_data;};
private:
    Map game_map_;
    TileMat tile_mat[MAX_TILES];
}; // quan ly viec xay dung map




#endif
