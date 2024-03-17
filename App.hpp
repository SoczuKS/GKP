#pragma once
#include <string>
#include <SDL2/SDL.h>

class App {
public:
    App();
    ~App();

    int run();

private:
    SDL_Window* window;
    SDL_Surface* screen;
    const int width;
    const int height;

    void clearScreen(Uint8, Uint8, Uint8) const;
    void setPixel(int, int, Uint8, Uint8, Uint8) const;
    void setPixel(int, int, const SDL_Color&) const;
    [[nodiscard]] SDL_Color getPixel(int, int) const;
    void loadBmp(const std::string&, int, int);
    void updateWindow() const;
};
