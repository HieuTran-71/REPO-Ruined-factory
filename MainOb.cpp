
#include "MainOb.h"

MainOb::MainOb()
{
    frame_ = 0;
    x_pos_ = 0;
    y_pos_ = 0;
    x_val_ = 0;
    y_val_ = 0;
    width_frame_ = 0;
    height_frame_ = 0;
    status_ = Walk_Right;
    input_type_.jump_ = 0;
    input_type_.down_ = 0;
    input_type_.up_ = 0;
    on_ground_ = false;
    map_x_ = 0;
    map_y_ = 0;
    jump_count_ = 0;
    come_back_time_ = 0;
    money_count = 0;
}

MainOb::~MainOb()
{

}

bool MainOb::LoadImg(std::string path, SDL_Renderer *screen)
{
    bool ret = BaseObject::LoadImg(path, screen);

    if (ret == true)
    {
        width_frame_ = 90;
        height_frame_ = rect_.h;
    }

    return ret;
}

SDL_Rect MainOb::GetRectFrame()
{
    SDL_Rect rect;
    rect.x = rect_.x;
    rect.y = rect_.y;
    rect.w = width_frame_;
    rect.h = height_frame_;

    return rect;
}

void MainOb::set_clips()
{
    if (width_frame_ > 0 && height_frame_ > 0)
    {
        for (int i = 0; i < 16; i++)
        {
            frame_clip_[i].x = i * width_frame_;
            frame_clip_[i].y = 0;
            frame_clip_[i].w = width_frame_;
            frame_clip_[i].h = height_frame_;
        }

    }
}

void MainOb::IncreaseSp()
{
    money_count++;
}

void MainOb::Show(SDL_Renderer* des, Map& map_data)
{
    LoadImg("image/move-R.png", des);

    frame_++;

    if (frame_ >= 16)
    {
        frame_ = 0;
    }

    if (come_back_time_ == 0)
    {
        rect_.x = x_pos_ - map_data.start_x_;
        rect_.y = y_pos_ - map_data.start_y_;

        SDL_Rect* current_clips = &frame_clip_[frame_];

        SDL_Rect renderQuad = {rect_.x, rect_.y, width_frame_, height_frame_};

        SDL_RenderCopy(des, p_object_, current_clips, &renderQuad);
    }

}

void MainOb::HandelInputAction(SDL_Event events, SDL_Renderer* screen)
{
    if (events.type == SDL_KEYDOWN)
    {
        switch (events.key.keysym.sym)
        {
        case SDLK_SPACE:
            if (jump_count_ < max_jump_)
            {
                y_val_ = -PLAYER_JUMP_VAL;  // Nhảy lên
                jump_count_++;              // Tăng số lần nhảy
                on_ground_ = false;         // Đánh dấu nhân vật không đứng trên đất
            }
            break;
        }
    }
    else if (events.type == SDL_KEYUP)
    {
        switch (events.key.keysym.sym)
        {
         case SDLK_SPACE:
            input_type_.jump_ = 0;
            break;
        }
    }

    if (events.type == SDL_KEYDOWN)
    {
        if (events.key.keysym.sym == SDLK_SPACE)
        {
            if (on_ground_)   // Chỉ cho phép nhảy khi đang đứng trên đất
            {
                input_type_.jump_ = 1;
            }
        }
    }

    if (events.type == SDL_KEYUP)
    {
        if (events.key.keysym.sym == SDLK_SPACE)
        {
            input_type_.jump_ = 0; // Reset trạng thái nhảy khi nhả phím
        }
    }

    if (events.type == SDL_MOUSEBUTTONDOWN)
    {
        if (events.button.button == SDL_BUTTON_LEFT)
        {

            Bullet* p_bullet = new Bullet(); //tao ra vien dan
            p_bullet->set_bullet_type(Bullet::SPHERE_BULLET);
            p_bullet->LoadImgBullet(screen);



            if (status_ == Walk_Left)
            {
                p_bullet->set_bullet_dir(Bullet::DIR_LEFT);
                p_bullet->SetRect(this->rect_.x, rect_.y + height_frame_*0.35); // set vi tri dan khi ban
            }
            else if(status_ == Walk_Right)
            {
                p_bullet->set_bullet_dir(Bullet::DIR_RIGHT);
                p_bullet->SetRect(this->rect_.x, rect_.y + height_frame_*0.35);
            }


            p_bullet->set_x_val(15); // toc do di chuyen cua dan
            p_bullet->set_y_val(15);
            p_bullet->set_is_move(true);

            p_bullet_list_.push_back(p_bullet);
        }
    }

}

void MainOb::HandleBullet(SDL_Renderer* des, const Map& map_data, SDL_Renderer* screen, ExplosionOb* exp_bullet)
{
    for (size_t i = 0 ; i < p_bullet_list_.size(); i++)
    {
        Bullet* p_bullet = p_bullet_list_.at(i);
        if (p_bullet != NULL)
        {
             if (p_bullet->get_is_move() == true)
            {
                p_bullet->HandleMove(SCREEN_WIDTH, SCREEN_HEIGHT);
                p_bullet->Render(des);
            }
            else
            {
                p_bullet_list_.erase(p_bullet_list_.begin() + i);
                i--;
                if (p_bullet != NULL)
                {
                    delete p_bullet;
                    p_bullet = NULL;
                }
            }
        }
    }
}

void MainOb::RemoveBullet(const int& idx)
{
    int size = p_bullet_list_.size();
    if (size > 0 && idx < size)
    {
        Bullet* p_bullet = p_bullet_list_.at(idx);
        p_bullet_list_.erase(p_bullet_list_.begin() + idx);

        if (p_bullet)
        {
            delete p_bullet;
            p_bullet = NULL;
        }
    }
}

void MainOb::Do_Player(Map& map_data)
{
    if (come_back_time_ == 0)
    {
        x_val_ = PLAYER_SPEED;
        y_val_ += GRAVITY;

        if (y_val_ >= MAX_FALL_SPEED)
        {
            y_val_ = MAX_FALL_SPEED;
        }


        if (input_type_.jump_ == 1 && on_ground_)
        {
            y_val_= - PLAYER_JUMP_VAL;
            on_ground_ = false;
        }

        CheckToMap(map_data);
        CenterEntityOnMap(map_data);
    }



}

void MainOb::CenterEntityOnMap(Map& map_data)
{
    map_data.start_x_ = x_pos_ - (SCREEN_WIDTH/4);
    if (map_data.start_x_ < 0)
    {
        map_data.start_x_ = 0;
    }
    else if (map_data.start_x_ + SCREEN_WIDTH >= map_data.max_x_)
    {
        map_data.start_x_ = map_data.max_x_ - SCREEN_WIDTH;
    }

    map_data.start_y_ = y_pos_ - (SCREEN_HEIGHT/4);
    if (map_data.start_y_ < 0)
    {
        map_data.start_y_ = 0;
    }
    else if (map_data.start_y_ + SCREEN_HEIGHT >= map_data.max_y_)
    {
        map_data.start_y_ = map_data.max_y_ - SCREEN_HEIGHT;
    }

}

void MainOb::CheckToMap(Map& map_data)
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

            int val1 = map_data.tile[y1][x2];
            int val2 = map_data.tile[y2][x2];

            if (val1 == STATE_SUPPORT || val2 == STATE_SUPPORT)
            {
                map_data.tile[y1][x2] = 0;
                map_data.tile[y2][x2] = 0;
                IncreaseSp();
            }
            else
            {
                if (val1 != TILE_TYPE_BLANK || val2 != TILE_TYPE_BLANK)
                {
                    x_pos_ = x2 * TILE_SIZE - width_frame_; // Căn nhân vật vào sát mép trái tile
                    x_val_ = 0;
                }
            }

        }
        else if (x_val_ < 0) // Di chuyển sang trái
        {
            int val1 = map_data.tile[y1][x1];
            int val2 = map_data.tile[y2][x1];

            if (val1 == STATE_SUPPORT || val2 == STATE_SUPPORT)
            {
                map_data.tile[y1][x1] = 0;
                map_data.tile[y2][x1] = 0;
                IncreaseSp();
            }
            else
            {
                if (val1 != TILE_TYPE_BLANK || val2 != TILE_TYPE_BLANK)
                {
                    x_pos_ = (x1 + 1) * TILE_SIZE; // Đặt nhân vật ở mép phải tile
                    x_val_ = 0;
                }
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
            int val1 = map_data.tile[y2][x1];
            int val2 = map_data.tile[y2][x2];

            if (val1 == STATE_SUPPORT || val2 == STATE_SUPPORT)
            {
                map_data.tile[y2][x1] = 0;
                map_data.tile[y2][x2] = 0;
                IncreaseSp();
            }
            else
            {
                if (val1 != TILE_TYPE_BLANK || val2 != TILE_TYPE_BLANK)
                {
                    y_pos_ = y2 * TILE_SIZE - height_frame_; // Căn nhân vật lên trên tile
                    y_val_ = 0;
                    on_ground_ = true; // Đánh dấu nhân vật đang đứng trên mặt đất
                    jump_count_ = 0; // reset so lan nhay
                }
                else
                {
                    on_ground_ = false; // Không va chạm => đang rơi
                }
            }

        }

    }

    x_pos_ += x_val_;
    y_pos_ += y_val_;

    if (x_pos_ < 0)
    {
        x_pos_ = 0;
    }
    else if (x_pos_ + width_frame_ > map_data.max_x_)
    {
        x_pos_ = map_data.max_x_ - width_frame_;
    }


}
