#include "App.hpp"
#include <iostream>
#include <random>
#include <string>

namespace {
SDL_Color getPixelFromSurface(const int x, const int y, const SDL_Surface* const surface) {
    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) {
        return {};
    }

    SDL_Color color{};
    Uint32 col{0};
    auto pPosition = static_cast<char*>(surface->pixels);
    pPosition += surface->pitch * y;
    pPosition += surface->format->BytesPerPixel * x;
    memcpy(&col, pPosition, surface->format->BytesPerPixel);
    SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);

    return color;
}
}

App::App() : width{640}, height{400}
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    window = SDL_CreateWindow("GKProject", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * 2, height * 2, SDL_WINDOW_SHOWN);

    if (nullptr == window) {
        throw std::runtime_error(SDL_GetError());
    }

    screen = SDL_GetWindowSurface(window);
    if (nullptr == screen) {
        throw std::runtime_error(SDL_GetError());
    }
}

App::~App() {
    if (screen) {
        SDL_FreeSurface(screen);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}


int App::run() {
    bool running{true};
    updateWindow();
    SDL_Event event;

    while(running && SDL_WaitEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;

            default:
                break;
            }
        }

        default:
            break;
        }
    }

    return EXIT_SUCCESS;
}

void App::clearScreen(const Uint8 r, const Uint8 g, const Uint8 b) const {
    SDL_FillRect(screen, nullptr, SDL_MapRGB(screen->format, r, g, b));
    updateWindow();
}

void App::setPixel(const int x, const int y, const SDL_Color& color) const {
    setPixel(x, y, color.r, color.g, color.b);
}

void App::setPixel(const int x, const int y, const Uint8 r, const Uint8 g, const Uint8 b) const {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        std::cerr << "Position out of range!\n";
        return;
    }

    Uint32 pixel = SDL_MapRGB(screen->format, r, g, b);
    int bpp = screen->format->BytesPerPixel;
    Uint8 *p1 = static_cast<Uint8*>(screen->pixels) + y * 2 * screen->pitch + x * 2 * bpp;
    Uint8 *p2 = static_cast<Uint8*>(screen->pixels) + (y * 2 + 1) * screen->pitch + x * 2 * bpp;
    Uint8 *p3 = static_cast<Uint8*>(screen->pixels) + y * 2 * screen->pitch + (x * 2 + 1) * bpp;
    Uint8 *p4 = static_cast<Uint8*>(screen->pixels) + (y * 2 + 1) * screen->pitch + (x * 2 + 1) * bpp;

    switch(bpp)
    {
    case 1:
        *p1 = pixel;
        *p2 = pixel;
        *p3 = pixel;
        *p4 = pixel;
        break;

    case 2:
        *reinterpret_cast<Uint16*>(p1) = pixel;
        *reinterpret_cast<Uint16*>(p2) = pixel;
        *reinterpret_cast<Uint16*>(p3) = pixel;
        *reinterpret_cast<Uint16*>(p4) = pixel;
        break;

    case 3:
        if constexpr(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p1[0] = pixel >> 16 & 0xff;
            p1[1] = pixel >> 8 & 0xff;
            p1[2] = pixel & 0xff;
            p2[0] = pixel >> 16 & 0xff;
            p2[1] = pixel >> 8 & 0xff;
            p2[2] = pixel & 0xff;
            p3[0] = pixel >> 16 & 0xff;
            p3[1] = pixel >> 8 & 0xff;
            p3[2] = pixel & 0xff;
            p4[0] = pixel >> 16 & 0xff;
            p4[1] = pixel >> 8 & 0xff;
            p4[2] = pixel & 0xff;
        } else {
            p1[0] = pixel & 0xff;
            p1[1] = pixel >> 8 & 0xff;
            p1[2] = pixel >> 16 & 0xff;
            p2[0] = pixel & 0xff;
            p2[1] = pixel >> 8 & 0xff;
            p2[2] = pixel >> 16 & 0xff;
            p3[0] = pixel & 0xff;
            p3[1] = pixel >> 8 & 0xff;
            p3[2] = pixel >> 16 & 0xff;
            p4[0] = pixel & 0xff;
            p4[1] = pixel >> 8 & 0xff;
            p4[2] = pixel >> 16 & 0xff;
        }
        break;

    case 4:
        *reinterpret_cast<Uint32*>(p1) = pixel;
        *reinterpret_cast<Uint32*>(p2) = pixel;
        *reinterpret_cast<Uint32*>(p3) = pixel;
        *reinterpret_cast<Uint32*>(p4) = pixel;
        break;

    default:
        break;
    }
}

SDL_Color App::getPixel(const int x, const int y) const {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return {};
    }

    SDL_Color color{};
    Uint32 col{0};
    auto pPosition = static_cast<char*>(screen->pixels);
    pPosition += screen->pitch * y * 2;
    pPosition += screen->format->BytesPerPixel * x * 2;
    memcpy(&col, pPosition, screen->format->BytesPerPixel);
    SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);

    return color;
}

void App::loadBmp(const std::string& filename, const int x, const int y) {
    SDL_Surface* bmp = SDL_LoadBMP(filename.c_str());
    if (nullptr == bmp) {
        std::cerr << "Unable to load bmp: " << filename << '\n';
        return;
    }

    for (int p{0}; p < bmp->w; ++p) {
        for (int q{0}; q < bmp->h; ++q) {
            auto color = getPixelFromSurface(p, q, bmp);
            setPixel(p, q, color);
        }
    }
    updateWindow();
    SDL_FreeSurface(bmp);
}

void App::updateWindow() const {
    SDL_UpdateWindowSurface(window);
}
