#include "CommonFunc.h"

bool SDLCommonFunc::CheckCollision(const SDL_Rect& object1, const SDL_Rect& object2)
{

    // Kiểm tra các trường hợp không va chạm
    if (object1.x + object1.w <= object2.x ||  // object1 nằm hoàn toàn bên trái object2
        object1.x >= object2.x + object2.w ||  // object1 nằm hoàn toàn bên phải object2
        object1.y + object1.h <= object2.y ||  // object1 nằm hoàn toàn phía trên object2
        object1.y >= object2.y + object2.h)    // object1 nằm hoàn toàn phía dưới object2
    {
        return false; // Không có va chạm
    }

    // Nếu không rơi vào bất kỳ trường hợp nào ở trên, nghĩa là có va chạm
    return true;
}
