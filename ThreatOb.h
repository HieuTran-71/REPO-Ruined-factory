
#ifndef THREAT_OB_H_
#define THREAT_OB_H_

#include "CommonFunc.h"
#include "BaseObject.h"

#define THREAT_FRAME_NUM 10
#define THREAT_GRAVITY_SPEED 0.8
#define THREAT_MAX_FALL_SPEED 10
#define THREAT_SPEED 3

class ThreatOb : public BaseObject
{
public:
    ThreatOb();
    ~ThreatOb();

    enum TypeMove
    {
        STATIC_THREAT = 0,
        MOVE_IN_SPACE = 1,
    };

    void set_x_val(const float& xVal) {x_val_ = xVal;}
    void set_y_val(const float& yVal) {y_val_ = yVal;}

    void set_x_pos(const float& xp) {x_pos_ = xp;}
    void set_y_pos(const float& yp) {y_pos_ = yp;}
    float get_x_pos() const {return x_pos_;}
    float get_y_pos() const {return y_pos_;}
    void SetMapXY(const int& mp_x, const int& mp_y) {map_x_ = mp_x; map_y_ = mp_y;}

    void set_clips();
    bool LoadImg(std::string path, SDL_Renderer* screen);
    void Show(SDL_Renderer* des);
    int get_width_frame() const {return width_frame_;}
    int get_height_frame() const {return height_frame_;}
    void DoPlayer(Map& gMap);
    void CheckToMap(Map& gMap);
    void set_type_move(const int& TypeMove) {type_move_ = TypeMove;}
    void SetAnimation(const int& pos_a, const int& pos_b) {animation_a_ = pos_a, animation_b_ = pos_b;}
    void set_input_right(const int& ipRight) {input_type_.right_ = ipRight;};
    void ImpMoveType(SDL_Renderer* screen);
    SDL_Rect GetRectFrame();

    void set_level(int lv) { level_ = lv; }
    int get_level() const { return level_; }

private:

    int level_;
    int map_x_;
    int map_y_;
    float y_val_;
    float x_val_;
    float x_pos_;
    float y_pos_;
    bool on_ground_;
    SDL_Rect frame_clip_[THREAT_FRAME_NUM];
    int width_frame_;
    int height_frame_;
    int frame_;
    int type_move_;
    int animation_a_;
    int animation_b_;
    Input input_type_;
};

#endif
