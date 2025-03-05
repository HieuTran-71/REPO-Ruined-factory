#ifndef MAIN_OB_H_
#define MAIN_OB_H_

#include "BaseObject.h"
#include "CommonFunc.h"

#define GRAVITY_SPEED 0.8 //toc do roi
#define MAX_FALL_SPEED 10 //toc do roi toi da
#define PLAYER_SPEED 8 // toc do nhan vat

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
    void Show(SDL_Renderer* des);
    void HandelInputAction(SDL_Event events, SDL_Renderer* screen);
    void set_clips(); // xu li ve animation

    void Do_Player(Map& map_data);// truyen map
    void CheckToMap(Map& map_data); // check va cham voi map
    void SetMapXY(const int map_x, const int map_y) {map_x_ = map_x; map_y_ = map_y;}
    void CenterEntityOnMap(Map& map_data); // ham xy ly thong so map khi di chuyen nhan vat
private:
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
};







#endif
