// ========================================================================== //
// Depenencies

// STL
#include <iostream>
#include <exception>

#include <functional>

#include <string>
using namespace std::string_literals;

// SDL
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>

// local
#include "window.hpp"

// ========================================================================== //
// local macro

#define THROWTEXT(msg) ("RUNTIME EXCEPTION IN "s + (__PRETTY_FUNCTION__) + "\n"s + msg)

#ifdef DEBUG
#define CHECK_ANIMATIONLAYER_INDEX(ID) { \
        if ( (ID >= this->animationLayers.size()) ) { \
            throw std::out_of_range(THROWTEXT( "  Invalid AnimationLayer ID: "s + std::to_string(ID) )); \
        } \
    }
#else
#define CHECK_ANIMATIONLAYER_INDEX(ID) {}
#endif

// ========================================================================== //
// namespace

namespace RetrogameBase
{

// ========================================================================== //
// CTor, DTor

    Window::Window(const char* title, int width, int height, Uint32 render_flags) :
        textureStore(*this),
        animationStore(*this),
        animationLayerStore(*this)
    {
        hwin = SDL_CreateWindow(title,
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,     // x, y
                                width, height,
                                SDL_WINDOW_SHOWN);

        win_renderer = SDL_CreateRenderer(hwin, -1, render_flags);
        // -1 is "index of driver"; -1 stands for "first supporting driver"

        SDL_SetRenderDrawBlendMode(win_renderer, SDL_BLENDMODE_BLEND);

        if (!(hwin && win_renderer))
        {
            throw SdlInternalError("Window could not be initialized.");
        }
    }

    Window::~Window()
    {
        SDL_DestroyRenderer(win_renderer);
        SDL_DestroyWindow(hwin);

        hwin         = nullptr;
        win_renderer = nullptr;
    }

// ========================================================================== //
// getters

    const char* Window::getTitle() const
    {
        return SDL_GetWindowTitle(hwin);
    }

    int Window::getWidth() const
    {
        return SDL_GetWindowSurface(hwin)->w;
    }

    int Window::getHeight() const
    {
        return SDL_GetWindowSurface(hwin)->h;
    }

    std::pair<int, int> Window::getDimension() const
    {
        int w, h;
        SDL_GetWindowSize(hwin, &w, &h);

        return std::make_pair(w, h);
    }

    int Window::getPosX() const
    {
        return getPosition().first;
    }

    int Window::getPosY() const
    {
        return getPosition().second;
    }

    std::pair<int, int> Window::getPosition() const
    {
        int x, y;
        SDL_GetWindowPosition(hwin, &x, &y);
        return std::make_pair(x, y);
    }

    Uint32 Window::GetWindowFlags() const
    {
        return SDL_GetWindowFlags(hwin);
    }

    SDL_Window* Window::getSdlWindow() const
    {
        return hwin;
    }

    SDL_Renderer* Window::getRenderer() const
    {
        return win_renderer;
    }

// ========================================================================== //
// place, hide and show

    void Window::setTitle(const char* title) const
    {
        SDL_SetWindowTitle(hwin, title);
    }

    void Window::setTitle(const std::string& title) const
    {
        setTitle(title.c_str());
    }

    void Window::setDimension(const int w, const int h) const
    {
        SDL_SetWindowSize(hwin, w, h);
    }

    void Window::setPosition(const int x, const int y) const
    {
        SDL_SetWindowPosition(hwin, x, y);
    }

    void Window::hide() const
    {
        SDL_HideWindow(hwin);
    }

    void Window::show() const
    {
        SDL_ShowWindow(hwin);
    }

    void Window::minimize() const
    {
        SDL_MinimizeWindow(hwin);
    }

    void Window::maximize() const
    {
        SDL_MaximizeWindow(hwin);
    }

    void Window::restore() const
    {
        SDL_RestoreWindow(hwin);
    }

    void Window::render() const
    {
        if (idleHandler)
        {
            idleHandler(userData);
        }
    }

    void Window::update() const
    {
        SDL_RenderPresent(win_renderer);
    }

// ========================================================================== //
// drawing primitives

    void Window::clear(SDL_Color color) const
    {
        SDL_SetRenderDrawColor( win_renderer, color.r, color.g, color.b, color.a );
        SDL_RenderClear       ( win_renderer );
    }

    void Window::pset(int x, int y, SDL_Color color) const
    {
        SDL_SetRenderDrawColor( win_renderer, color.r, color.g, color.b, color.a );
        SDL_RenderDrawPoint( win_renderer, x, y);
    }

    void Window::line(int x1, int y1, int x2, int y2, SDL_Color color) const
    {
        SDL_SetRenderDrawColor( win_renderer, color.r, color.g, color.b, color.a );
        SDL_RenderDrawLine(win_renderer, x1, y1, x2, y2);
    }

    void Window::box(int x, int y, int w, int h, SDL_Color color) const
    {
        SDL_SetRenderDrawColor( win_renderer, color.r, color.g, color.b, color.a );
        SDL_Rect rect = { x, y, w, h };
        SDL_RenderDrawRect(win_renderer, &rect);
    }

    void Window::fbox(int x, int y, int w, int h, SDL_Color color) const
    {
        SDL_SetRenderDrawColor( win_renderer, color.r, color.g, color.b, color.a );
        SDL_Rect rect = { x, y, w, h };
        SDL_RenderFillRect(win_renderer, &rect);
    }

    void Window::print(
        const char* text,
        const int x, const int y,
        int width, int height,
        SDL_Color color,
        TTF_Font* font
    ) const
    {
        if (!text)
        {
            return;
        }

        if ( !font )
        {
            font = fonts["fixed-small"];
        }

        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
        SDL_Texture* msg = SDL_CreateTextureFromSurface(win_renderer, surfaceMessage);

        if (width  == -1)
        {
            width = surfaceMessage->w;
        }
        if (height == -1)
        {
            height = surfaceMessage->h;
        }

        SDL_Rect msg_rect = {x, y, width, height};

        SDL_RenderCopy(win_renderer, msg, NULL, &msg_rect);

        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(msg);
    }

    void Window::saveScreenshotPNG(const std::string& filename) const
    {
        const auto widthAndHeight = getDimension();
        const SDL_Rect coordinates = {0, 0, widthAndHeight.first, widthAndHeight.second};

        saveScreenshotPNG(filename, coordinates);
    }

    void Window::saveScreenshotPNG(const std::string& filename, const SDL_Rect& coordinates) const
    {
        SDL_Surface* sshot = SDL_CreateRGBSurface(
                                 0,                                 // flags -- must be zero
                                 coordinates.w, coordinates.h,      // dimensions
                                 32,                                // bits per pixel
                                 0x00ff0000,                        // masks for R, G, B, A
                                 0x0000ff00,
                                 0x000000ff,
                                 0xff000000
                             );

        SDL_RenderReadPixels(
            win_renderer,
            &coordinates,
            SDL_PIXELFORMAT_ARGB8888,
            sshot->pixels,
            sshot->pitch
        );

        IMG_SavePNG(sshot, filename.c_str());
        SDL_FreeSurface(sshot);
    }

// ========================================================================== //
// storage access

    TextureStore& Window::getTextureStore()
    {
        return textureStore;
    }

    AnimationStore& Window::getAnimationStore()
    {
        return animationStore;
    }

    AnimationLayerStore& Window::getAnimationLayerStore()
    {
        return animationLayerStore;
    }

    void Window::resetStores(ResetStoresDepth depth)
    {
        switch (depth)
        {
            case ResetStoresDepth::Textures:
                textureStore.reset_private();
                [[fallthrough]];
            case ResetStoresDepth::Animations:
                animationStore.reset_private();
                [[fallthrough]];
            case ResetStoresDepth::Layers:
                animationLayerStore.reset_private();
        }
    }

// ========================================================================== //
// event handling

    void Window::setEventHandler(const std::function<bool (SDL_Event&, void*)>& eventHandler)
    {
        this->eventHandler = eventHandler;
    }

    const std::function<bool (SDL_Event& event, void* userData)> Window::getEventHandler() const
    {
        return eventHandler;
    }

    void Window::setIdleHandler(const std::function<void (void*)>& idleHandler)
    {
        this->idleHandler = idleHandler;
    }

    const std::function<void (void* userData)> Window::getIdleHandler() const
    {
        return idleHandler;
    }

    bool Window::distributeEvents()
    {
        if (!eventHandler)
        {
            return false;
        }

        SDL_Event event;
        bool continueToLoop = true;

        while (SDL_PollEvent(&event))
        {
            continueToLoop &= eventHandler(event, userData);
        }

        return continueToLoop;
    }

    void Window::mainLoop(double fps)
    {
        bool continueToLoop = bool(eventHandler);

        const int delay = 1000 / fps;

        while (continueToLoop)
        {
            render();
            continueToLoop = distributeEvents();
            update();

            SDL_Delay(delay);
        }
    }

    void* Window::getUserData() const
    {
        return userData;
    }

    void Window::setUserData(void* const newUserData)
    {
        userData = newUserData;
    }

// ========================================================================== //
// namespace

}
