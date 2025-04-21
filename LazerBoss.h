
#ifndef LAZERBOSS_H_
#define LAZERBOSS_H_

#define LASER_SPEED 2


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
    void Update();

    SDL_Rect GetRect();

    int get_x_pos() const { return x_pos_; }
    int get_y_pos() const { return y_pos_; }

    int get_width() const { return width_; }
    int get_height() const { return height_; }

    int get_height_frame() const { return height_; } // Đã có, nhưng cần đảm bảo đúng
    bool is_active() const { return is_active_; }

    // Setters (hàm public để thay đổi giá trị các biến private)
    void set_x_pos(int x) { x_pos_ = x; }
    void set_y_pos(int y) { y_pos_ = y; }

    void set_is_active(bool is_active) { is_active_ = is_active; }
    bool get_is_active() const {return is_active_;}
private:
    int x_pos_;
    int y_pos_;
    int width_;
    int height_;
    bool is_active_; // Có đang hoạt động không?
    int map_x_;
    int map_y_;
};


#endif
