
#include "LazerBoss.h"

LaserBeam::LaserBeam()
{
    x_pos_ = 0;
    y_pos_ = 0;
    width_ = 0;
    height_ = 0;
    is_active_ = true;
    map_x_ = 0;
    map_y_ = 0;
}

LaserBeam::~LaserBeam()
{
   Free();
}

bool LaserBeam::LoadImg(const std::string& path, SDL_Renderer* screen) {
    bool success = BaseObject::LoadImg(path, screen);
    if (success) {
        width_ = rect_.w;
        height_ = rect_.h;
    }
    return success;
}

void LaserBeam::Show(SDL_Renderer* screen) {
    if (is_active_) {
        SDL_Rect renderRect = {x_pos_ , y_pos_ , width_, height_};
        Render(screen, &renderRect);
    }
}

void LaserBeam::SetMapXY(int map_x, int map_y) {
    map_x_ = map_x;
    map_y_ = map_y;
}

void LaserBeam::Update() {
    if (is_active_) {
        x_pos_ += LASER_SPEED;
        if (x_pos_ > SCREEN_WIDTH)
        {
            x_pos_ = 0;
        }

        map_x_ = 0;
        map_y_ = 0;
    }
}


SDL_Rect LaserBeam::GetRect() {
    return {x_pos_, y_pos_, width_, height_};
}


