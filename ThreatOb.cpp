
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

SDL_Rect ThreatOb::GetRectFrame()
{
    SDL_Rect rect;
    rect.x = rect_.x;
    rect.y = rect_.y;
    rect.w = width_frame_;
    rect.h = height_frame_;

    return rect;
}

void ThreatOb::set_clips()
{
    if (width_frame_ > 0 && height_frame_ > 0)
    {
        for (int i = 0; i < THREAT_FRAME_NUM; i++)
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

    rect_.x = x_pos_ - map_x_;
    rect_.y = y_pos_ - map_y_;
    frame_++;
    if (frame_ >= THREAT_FRAME_NUM)
    {
        frame_ = 0;
    }
    SDL_Rect* currentClip = &frame_clip_[frame_];
    SDL_Rect rendQuad = {rect_.x, rect_.y, width_frame_, height_frame_};
    SDL_RenderCopy(des, p_object_, currentClip, &rendQuad);

}

void ThreatOb::DoPlayer(Map& gMap)
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


void ThreatOb::CheckToMap(Map& gMap)
{
    int x1 = 0;
    int x2 = 0;

    int y1 = 0;
    int y2 = 0;

    int height_min = height_frame_ < TILE_SIZE ? height_frame_ : TILE_SIZE;

    x1 = (x_pos_ + x_val_)/TILE_SIZE;
    x2 = (x_pos_ + x_val_ + width_frame_ - 1)/TILE_SIZE;

    y1 = (y_pos_) / TILE_SIZE;
    y2 = (y_pos_ + height_min - 1)/TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (x_val_ > 0)
        {

            int val1 = gMap.tile[y1][x2];
            int val2 = gMap.tile[y2][x2];
            if ((val1 != TILE_TYPE_BLANK && val1 != STATE_SUPPORT)||(val2 != STATE_SUPPORT && val2 != TILE_TYPE_BLANK))
            {
                x_pos_ = x2 * TILE_SIZE - width_frame_;
                x_val_ = 0;
            }


        }
        else if (x_val_ < 0)
        {
            int val1 = gMap.tile[y1][x1];
            int val2 = gMap.tile[y2][x1];
            if ((val1 != TILE_TYPE_BLANK && val1 != STATE_SUPPORT)||(val2 != STATE_SUPPORT && val2 != TILE_TYPE_BLANK))
            {
                x_pos_ = (x1 + 1) * TILE_SIZE;
                x_val_ = 0;
            }
        }
    }



    int width_min = width_frame_ < TILE_SIZE ? width_frame_ : TILE_SIZE;
    x1 = (x_pos_)/TILE_SIZE;
    x2 = (x_pos_ + width_min)/ TILE_SIZE;

    y1 = (y_pos_ + y_val_)/TILE_SIZE;
    y2 = (y_pos_ + y_val_ + height_frame_ - 1)/TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (y_val_ > 0)
        {
            int val1 = gMap.tile[y2][x1];
            int val2 = gMap.tile[y2][x2];

            if ((val1 != TILE_TYPE_BLANK && val1 != STATE_SUPPORT)||(val2 != STATE_SUPPORT && val2 != TILE_TYPE_BLANK))
            {
                y_pos_ = y2 * TILE_SIZE - height_frame_;
                y_val_ = 0;
                on_ground_ = true;
            }
            else
            {
                on_ground_ = false;
            }

        }
        else if (y_val_ < 0)
        {
            int val1 = gMap.tile[y1][x1];
            int val2 = gMap.tile[y1][x2];

            if ((val1 != TILE_TYPE_BLANK && val1 != STATE_SUPPORT)||(val2 != STATE_SUPPORT && val2 != TILE_TYPE_BLANK))
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

}

void ThreatOb::ImpMoveType(SDL_Renderer* screen)
{
    if (type_move_ == STATIC_THREAT)
    {
        return;
    }
    else
    {
        if (on_ground_ = true)
        {
            if (x_pos_ > animation_b_)
            {
                input_type_.left_ = 1;
                input_type_.right_ = 0;
                LoadImg("image/threat_Lv1_L .png", screen);
            }
            else if (x_pos_ < animation_a_)
            {
                input_type_.left_ = 0;
                input_type_.right_ = 1;
                LoadImg("image/threat_Lv1_R .png", screen);
            }
        }
    }
}
