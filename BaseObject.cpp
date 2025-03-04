#include "BaseObject.h"
#include <iostream>


BaseObject::BaseObject() {
    p_object_ = NULL;
    rect_.x = 0;
    rect_.y = 0;
    rect_.w = 0;
    rect_.h = 0;
}

BaseObject::~BaseObject() {
    Free();
}

bool BaseObject::LoadImg(std::string path, SDL_Renderer* screen) {
    SDL_Texture* new_texture = NULL;
    SDL_Surface* loaded_surface = IMG_Load(path.c_str());

    if (loaded_surface != NULL) {
        SDL_SetColorKey(loaded_surface, SDL_TRUE, SDL_MapRGB(loaded_surface->format, COLOR_KEY_R, COLOR_KEY_G, COLOR_KEY_B));
        new_texture = SDL_CreateTextureFromSurface(screen, loaded_surface);
        if (new_texture != NULL) {
            rect_.w = loaded_surface->w;
            rect_.h = loaded_surface->h;
        } else {
            std::cerr << "Unable to create texture from surface! SDL Error: " << SDL_GetError() << std::endl;
        }

        SDL_FreeSurface(loaded_surface);
    } else {
        std::cerr << "Unable to load image at " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    }

    p_object_ = new_texture;

    return p_object_ != NULL;
}

void BaseObject::Render(SDL_Renderer* des, const SDL_Rect* clip) {
    if (p_object_ != NULL) {
        SDL_Rect renderquad = { rect_.x, rect_.y, rect_.w, rect_.h };
        if (clip != NULL) {
            renderquad.w = clip->w;
            renderquad.h = clip->h;
        }
        SDL_RenderCopy(des, p_object_, clip, &renderquad);
    }
}

void BaseObject::Free() {
    if (p_object_ != NULL) {
        SDL_DestroyTexture(p_object_);
        p_object_ = NULL;
        rect_.w = 0;
        rect_.h = 0;
    }
}
