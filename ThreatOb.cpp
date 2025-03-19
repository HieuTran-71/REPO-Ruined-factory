
#include "ThreatOb.h"

ThreatOb::ThreatOb()
{
    width_frame_ = 0;
    height_frame_ = 0;
    x_val_ = 0;
    y_val_ = 0;
    x_pos_ = 0;
    y_pos_ = 0;
    on_ground_ = 0;
    frame_ = 0;
    come_back_time = 0;

    animation_a_ = 0;
    animation_b_ = 0;
    input_type_.left_ = 0;
    type_move_ = STATIC_THREAT;

}

ThreatOb::~ThreatOb()
{

}

bool ThreatOb::LoadImg(std::string path, SDL_Renderer* screen)
{
    bool ret = BaseObject::LoadImg(path, screen);
    if (ret)
    {
        width_frame_ = rect_.w / THREAT_FRAME_NUM;
        height_frame_ = rect_.h;

    }

    return ret;
}

void ThreatOb::set_clips()
{
    if (width_frame_ > 0 && height_frame_ > 0)
    {
        for (int i = 0; i < 7; i++)
        {
            frame_clip_[i].x = i * width_frame_;
            frame_clip_[i].y = 0;
            frame_clip_[i].w = width_frame_;
            frame_clip_[i].h = height_frame_;
        }

    }
}

void ThreatOb::Show(SDL_Renderer* des)
{
    if (come_back_time == 0)
    {
        rect_.x = x_pos_ - map_x_;
        rect_.y = y_pos_ - map_y_;
        frame_++;
        if (frame_ >= 7)
        {
            frame_ = 0;
        }
        SDL_Rect* currentClip = &frame_clip_[frame_];
        SDL_Rect rendQuad = {rect_.x, rect_.y, width_frame_, height_frame_};
        SDL_RenderCopy(des, p_object_, currentClip, &rendQuad);
    }
}

void ThreatOb::DoPlayer(Map& gMap)
{
    if (come_back_time == 0)
    {
        x_val_ = 0;
        y_val_ += THREAT_GRAVITY_SPEED;

        if (y_val_ >= THREAT_MAX_FALL_SPEED)
        {
            y_val_ = THREAT_MAX_FALL_SPEED;
        }

        if (input_type_.left_ == 1)
        {
            x_val_ -= THREAT_SPEED;
        }
        else if (input_type_.right_ == 1)
        {
            x_val_ += THREAT_SPEED;
        }

        CheckToMap(gMap);
    }
    else if (come_back_time > 0)
    {
        come_back_time--;
        if (come_back_time == 0)
        {
            InitThreats();
        }
    }
}

void ThreatOb::InitThreats()
{
    x_val_ = 0;
    y_val_ = 0;
    if (x_pos_ > 256)
    {
        x_pos_ -=256;
        animation_a_ -= 256;
        animation_b_ -= 256;
    }
    else
    {
        x_pos_ = 0;
    }
    y_pos_ = 0;
    come_back_time = 0;
    input_type_.left_ = 1;
}

void ThreatOb::CheckToMap(Map& gMap)
{
    int x1 = 0;
    int x2 = 0; // gioi han kiem tra theo chieu x

    int y1 = 0;
    int y2 = 0; // gioi han kiem tra theo chieu y

    //Check theo chieu ngang
    int height_min = height_frame_ < TILE_SIZE ? height_frame_ : TILE_SIZE;

    x1 = (x_pos_ + x_val_)/TILE_SIZE; // biet duoc nhan vat dang o o thu may
    x2 = (x_pos_ + x_val_ + width_frame_ - 1)/TILE_SIZE; // tru di mot luong nho de check va cham

    y1 = (y_pos_) / TILE_SIZE;
    y2 = (y_pos_ + height_min - 1)/TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (x_val_ > 0) // Di chuyển sang phải
        {

            int val1 = gMap.tile[y1][x2];
            int val2 = gMap.tile[y2][x2];
            if ((val1 != BLANK_TILE && val1 != STATE_SUPPORT)||(val2 != STATE_SUPPORT && val2 != BLANK_TILE))
            {
                x_pos_ = x2 * TILE_SIZE - width_frame_; // Căn nhân vật vào sát mép trái tile
                x_val_ = 0;
            }


        }
        else if (x_val_ < 0) // Di chuyển sang trái
        {
            int val1 = gMap.tile[y1][x1];
            int val2 = gMap.tile[y2][x1];
            if ((val1 != BLANK_TILE && val1 != STATE_SUPPORT)||(val2 != STATE_SUPPORT && val2 != BLANK_TILE))
            {
                x_pos_ = (x1 + 1) * TILE_SIZE; // Đặt nhân vật ở mép phải tile
                x_val_ = 0;
            }
        }
    }


    //check theo chieu doc
    int width_min = width_frame_ < TILE_SIZE ? width_frame_ : TILE_SIZE;
    x1 = (x_pos_)/TILE_SIZE;
    x2 = (x_pos_ + width_min)/ TILE_SIZE;

    y1 = (y_pos_ + y_val_)/TILE_SIZE;
    y2 = (y_pos_ + y_val_ + height_frame_ - 1)/TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (y_val_ > 0) // Rơi xuống
        {
            int val1 = gMap.tile[y2][x1];
            int val2 = gMap.tile[y2][x2];

            if ((val1 != BLANK_TILE && val1 != STATE_SUPPORT)||(val2 != STATE_SUPPORT && val2 != BLANK_TILE))
            {
                y_pos_ = y2 * TILE_SIZE - height_frame_; // Căn nhân vật lên trên tile
                y_val_ = 0;
                on_ground_ = true; // Đánh dấu nhân vật đang đứng trên mặt đất
            }
            else
            {
                on_ground_ = false; // Không va chạm => đang rơi
            }

        }
        else if (y_val_ < 0)
        {
            int val1 = gMap.tile[y1][x1];
            int val2 = gMap.tile[y1][x2];

            if ((val1 != BLANK_TILE && val1 != STATE_SUPPORT)||(val2 != STATE_SUPPORT && val2 != BLANK_TILE))
            {
                y_pos_ = (y1 + 1) * TILE_SIZE;
                y_val_ = 0;
            }
        }

    }

    x_pos_ += x_val_;
    y_pos_ += y_val_;

    if (x_pos_ < 0)
    {
        x_pos_ = 0;
    }
    else if (x_pos_ + width_frame_ > gMap.max_x_)
    {
        x_pos_ = gMap.max_x_ - width_frame_;
    }

    if (y_pos_ > gMap.max_y_)
    {
        come_back_time = 50;
    }

}

void ThreatOb::ImpMoveType(SDL_Renderer* screen)
{
    if (type_move_ == STATIC_THREAT)
    {
        ;
    }
    else
    {
        if (on_ground_ = true)
        {
            if (x_pos_ > animation_b_)
            {
                input_type_.left_ = 1;
                input_type_.right_ = 0;
                LoadImg("image/threat_Lv1_L.png", screen);
            }
            else if (x_pos_ < animation_a_)
            {
                input_type_.left_ = 0;
                input_type_.right_ = 1;
                LoadImg("image/threat_Lv1_R.png", screen);
            }
        }
        else
        {
            if (input_type_.left_ == 1)
            {
                LoadImg("image/threat_Lv1_L.png", screen);
            }
        }
    }
}
