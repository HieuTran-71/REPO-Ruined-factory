#include "TextOb.h"

TextOb::TextOb()
{
    text_color_.r = 255;
    text_color_.g = 255;
    text_color_.b = 255;
    texture_ = NULL;
    font_ = NULL; // Thêm khởi tạo font_
}

TextOb::~TextOb()
{
    Free();
}

void TextOb::SetFont(TTF_Font* font)
{
    font_ = font;
}

bool TextOb::LoadFromRenderText(TTF_Font* font, SDL_Renderer* screen)
{
    Free(); // Giải phóng texture cũ trước khi tạo mới
    if (font == NULL) {
        return false;
    }
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, str_val_.c_str(), text_color_);
    if (text_surface)
    {
        texture_ = SDL_CreateTextureFromSurface(screen, text_surface);
        width_ = text_surface->w;
        height_ = text_surface->h;

        SDL_FreeSurface(text_surface);
    }
    return texture_ != NULL;
}

void TextOb::Free()
{
    if (texture_ != NULL)
    {
        SDL_DestroyTexture(texture_);
        texture_ = NULL;
        width_ = 0;
        height_ = 0;
    }
}

void TextOb::SetColor(Uint8 red, Uint8 green, Uint8 blue)
{
    text_color_.r = red;
    text_color_.g = green;
    text_color_.b = blue;
}

void TextOb::SetColor(int type)
{
    if (type == RED_TEXT)
    {
        SDL_Color color = {255, 0 , 0};
        text_color_ = color;
    }
    else if (type == WHITE_TEXT)
    {
        SDL_Color color = {255, 255 , 255};
        text_color_ = color;
    }
    else if (type == BLACK_TEXT)
    {
        SDL_Color color = {0, 0 , 0};
        text_color_ = color;
    }
    else if (type == YELLOW_TEXT) // Thêm màu YELLOW_TEXT
    {
        SDL_Color color = {255, 255, 0};
        text_color_ = color;
    }
}

void TextOb::RenderText(SDL_Renderer* screen,
                        int xp, int yp,
                        SDL_Rect* clip ,
                        double angle,
                        SDL_Point* center ,
                        SDL_RendererFlip flip )
{
    SDL_Rect renderquad = {xp, yp, width_, height_};
    if (clip != NULL)
    {
        renderquad.w = clip->w;
        renderquad.h = clip->h;
    }

    SDL_RenderCopyEx(screen, texture_, clip, &renderquad, angle,center, flip);
}

