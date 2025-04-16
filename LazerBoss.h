
#ifndef LAZERBOSS_H_
#define LAZERBOSS_H_

#include "CommonFunc.h"
#include "MainOb.h"
#include "BaseObject.h"

class LaserBeam : public BaseObject
{
public:
    LaserBeam();
    ~LaserBeam();

    bool LoadImg(const std::string& path, SDL_Renderer* screen);
    void Show(SDL_Renderer* screen);
    void SetMapXY(int map_x, int map_y);
    void Update(const MainOb& player); // Cập nhật vị trí theo nhân vật

    SDL_Rect GetRect();

private:
    int x_pos_;
    int y_pos_;
    int width_;
    int height_;
    bool is_active_; // Có đang hoạt động không?
    int scroll_speed_; // Tốc độ quét từ trái sang phải
    int map_x_;
    int map_y_;
};


#endif
