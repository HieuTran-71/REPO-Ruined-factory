
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

void Bullet::HandleMove(const int& x_border, const int& y_border, const Map& map_data, SDL_Renderer* screen, ExplosionOb* exp_bullet)
{
    rect_.x += (bullet_dir_ == DIR_RIGHT) ? x_val_ : -x_val_;

    if (rect_.x < 0 || rect_.x > x_border)
    {
        is_move_ = false;
        return;
    }

    SDL_Rect bullet_rect = rect_;

    // Chỉ kiểm tra vùng tile xung quanh viên đạn
    int start_x = bullet_rect.x / TILE_SIZE - 1;
    int end_x = (bullet_rect.x + bullet_rect.w) / TILE_SIZE + 1;
    int start_y = bullet_rect.y / TILE_SIZE - 1;
    int end_y = (bullet_rect.y + bullet_rect.h) / TILE_SIZE + 1;

    start_x = std::max(0, start_x);
    end_x = std::min(MAX_MAP_X, end_x);
    start_y = std::max(0, start_y);
    end_y = std::min(MAX_MAP_Y, end_y);

    for (int y = start_y; y < end_y; y++)
    {
        for (int x = start_x; x < end_x; x++)
        {
            int tile_value = map_data.tile[y][x];

            if (tile_value > 0 && tile_value < MAX_TILES )
            {
                int tile_type = tile_type_mapping[tile_value];

                if (tile_type == TILE_TYPE_SOLID)
                {
                    SDL_Rect tile_rect;
                    tile_rect.x = x * TILE_SIZE;
                    tile_rect.y = y * TILE_SIZE;
                    tile_rect.w = TILE_SIZE;
                    tile_rect.h = TILE_SIZE;

                    if (SDLCommonFunc::CheckCollision(bullet_rect, tile_rect))
                    {
                        // Hiệu ứng nổ
                        for (int ex = 0; ex < NUM_FRAME_EXP; ex++)
                        {
                            int x_pos = rect_.x - exp_bullet->get_frame_width() * 0.07;
                            int y_pos = rect_.y - exp_bullet->get_frame_height() * 0.25;

                            exp_bullet->set_frame(ex);
                            exp_bullet->SetRect(x_pos, y_pos);
                            exp_bullet->Show(screen);
                            SDL_RenderPresent(screen);
                        }

                        is_move_ = false;
                        return;
                    }
                }
            }
        }
    }
}
