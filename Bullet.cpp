
#include "Bullet.h"

Bullet::Bullet()
{
    x_val_ = 0;
    y_val_ = 0;
    is_move_ = false;
    bullet_type_ = SPHERE_BULLET;

}

Bullet::~Bullet()
{


}
bool Bullet::LoadImgBullet(SDL_Renderer* des)
{
    bool ret = false;
    if (bullet_type_ == LIQUID_BULLET)
    {
        ret = LoadImg("image/Bullet_Threat.png", des);
    }
    else if (bullet_type_ == SPHERE_BULLET)
    {
        ret = LoadImg("image/Bullet.png", des);
    }

    return ret;
}

void Bullet::HandleMove(const int& x_border, const int& y_border)
{
    if (bullet_dir_ == DIR_RIGHT)
    {
        rect_.x += x_val_;
        if (rect_.x > x_border)
        {
            is_move_ = false;
        }
    }
}
