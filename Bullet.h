

#ifndef BULLET_H_
#define BULLET_H_

#include "BaseObject.h"
#include "CommonFunc.h"
#include "ExplosionOb.h"

class Bullet : public BaseObject
{
public:
    Bullet();
    ~Bullet();

    enum BulletDir
    {
        DIR_RIGHT = 20,
        DIR_LEFT = 21,


    };

    enum BulletType
    {
        SPHERE_BULLET = 50,
        LIQUID_BULLET = 51,
    };

    void set_x_val(const int& xVal) {x_val_ = xVal;}
    void set_y_val(const int& yVal) {y_val_ = yVal;}
    int get_x_val() const {return x_val_;}
    int get_y_val() const {return y_val_;}

    void set_is_move(const bool& isMove) {is_move_ = isMove;}
    bool get_is_move() const {return is_move_;}

    void set_bullet_dir(const unsigned int& bulletDir) {bullet_dir_ = bulletDir;}
    unsigned int get_bullet_dir() const {return bullet_dir_;}
    void HandleMove(const int& x_border, const int& y_border, const Map& map_data, SDL_Renderer* screen, ExplosionOb* exp_bullet);

    void set_bullet_type(const unsigned int& bulletType) {bullet_type_ = bulletType;}
    unsigned int get_bullet_type() const {return bullet_type_;}
    bool LoadImgBullet(SDL_Renderer* des);
private:
    int x_val_;
    int y_val_;
    bool is_move_;
    unsigned int bullet_dir_; // quan li huong cua duong dan
    unsigned int bullet_type_;
};





#endif
