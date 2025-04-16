
#include "LazerBoss.h"

LaserBeam::LaserBeam()
{
    x_pos_ = 0;
    y_pos_ = 0;
    width_ = 0;
    height_ = 0;
    is_active_ = false;
    scroll_speed_ = 2;
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
        SDL_Rect renderRect = {x_pos_ - map_x_, y_pos_ - map_y_, width_, height_};
        Render(screen, &renderRect);
    }
}

void LaserBeam::SetMapXY(int map_x, int map_y) {
    map_x_ = map_x;
    map_y_ = map_y;
}

void LaserBeam::Update(const MainOb& player) {
    if (is_active_) {
        x_pos_ += scroll_speed_;
        // Theo dõi vị trí Y của nhân vật (có thể có offset)
        y_pos_ = player.GetRect().y + player.GetRect().h / 2 - height_ / 2; // Giữa nhân vật

        // Kiểm tra nếu tia laze đã đi hết màn hình (hoặc một khoảng nhất định)
        if (x_pos_ > map_x_ + SCREEN_WIDTH) {
            is_active_ = false; // Tắt tia laze khi đi hết
        }
    }
}

SDL_Rect LaserBeam::GetRect() {
    return {x_pos_, y_pos_, width_, height_};
}

