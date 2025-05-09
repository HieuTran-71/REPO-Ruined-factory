#ifndef MAIN_OB_H_
#define MAIN_OB_H_

#include <vector>
#include "BaseObject.h"
#include "CommonFunc.h"
#include "Bullet.h"
#include "SoundManager.h"


#define GRAVITY_SPEED 0.8 //toc do roi
#define MAX_FALL_SPEED 10 //toc do roi toi da
#define PLAYER_SPEED 6 // toc do nhan vat
#define PLAYER_JUMP_VAL 15
#define GRAVITY 0.8

class MainOb : public BaseObject
{
public:
    MainOb();
    ~MainOb();

    enum WalkType
    {
        Walk_Right = 0,
        Walk_Left = 1,
    };
    bool LoadImg(std::string path, SDL_Renderer* screen);
    void Show(SDL_Renderer* des, Map& map_data);
    void HandelInputAction(SDL_Event events, SDL_Renderer* screen);
    void set_clips(); // xu li ve animation

    void Do_Player(Map& map_data);// truyen map
    void CheckToMap(Map& map_data); // check va cham voi map
    void SetMapXY(const int map_x, const int map_y) {map_x_ = map_x; map_y_ = map_y;}
    void CenterEntityOnMap(Map& map_data); // ham xy ly thong so map khi di chuyen nhan vat
    void UpdateImagePlayer(SDL_Renderer* des); // xu li dan ban
    SDL_Rect GetRectFrame() const;

    void set_bullet_list(std::vector<Bullet*> bullet_list)
    {
        p_bullet_list_ = bullet_list;
    }
    std::vector<Bullet*> get_bullet_list() const {return p_bullet_list_;}
    void HandleBullet(SDL_Renderer* des, const Map& map_data, SDL_Renderer* screen, ExplosionOb* exp_bullet);
    void RemoveBullet(const int& idx);
    void IncreaseSp(); // ktra tang so luong vat pham ho tro
    void set_comeback_time(const int& cb_time) {come_back_time_ = cb_time;}
    int GetMoney() const {return money_count;}

    void SetXPos(const float& x_pos) { x_pos_ = x_pos; }
    void SetYPos(const float& y_pos) { y_pos_ = y_pos; }
    void SetMapX(const int& map_x) { map_x_ = map_x; }
    void SetMapY(const int& map_y) { map_y_ = map_y; }
    float GetXPos() const { return x_pos_; }
    float GetYPos() const { return y_pos_; }

    void SetPos(const float& x, const float& y)
    {
        x_pos_ = x;
        y_pos_ = y;
    }

    SDL_Rect GetCollisionBox() const;

    void SetMarkValuePtr(UINT* mark_value) {
        mark_value_ptr_ = mark_value;
    }

    void AddScore(UINT value) {
        if (mark_value_ptr_ != nullptr) {
            *mark_value_ptr_ += value;
        }
    }

private:
    int money_count; // vat pham ho tro

    std::vector<Bullet*> p_bullet_list_; //bang dan

    float x_val_;
    float y_val_;

    float x_pos_;
    float y_pos_;

    int width_frame_;
    int height_frame_;

    SDL_Rect frame_clip_[16];
    Input input_type_;
    int frame_;
    int status_;//luu trang thai cua 2 kieu di chuyen
    bool on_ground_; //luu trang thai tren mat dat

    int map_x_;
    int map_y_;

    int jump_count_; // Đếm số lần nhảy
    const int max_jump_ = 2; // Số lần nhảy tối đa (2 lần)

    int come_back_time_;

    UINT* mark_value_ptr_;


};







#endif
