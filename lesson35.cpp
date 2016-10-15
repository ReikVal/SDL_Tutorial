#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Renderer *renderer;

class Texture {
    public:
        ~Texture();

        bool loadFromFile(string path);
        void free();
        void render(int x, int y);

        int getWidth();
        int getHeight();
    private:
        SDL_Texture *texture;
        int width{0};
        int height{0};
};

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(string path) {
    SDL_Surface *surf = IMG_Load(path.c_str());
    if(surf == nullptr) {
        cout << IMG_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        if(texture == nullptr) {
            cout << SDL_GetError() << endl;
        } else {
            width = surf->w;
            height = surf->h;
        }
        SDL_FreeSurface(surf);
    }
    return texture != nullptr;
}

void Texture::free() {
    if(texture != nullptr) {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

void Texture::render(int x, int y) {
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

//Wrapper para la ventana:
class Window {
    public:
        //Crea la ventana:
        bool init();

        //Crea el renderer para la ventana interna:
        SDL_Renderer *createRenderer();

        //Handler de los eventos de ventana:
        void handleEvent(SDL_Event &e);

        //Libera recursos:
        void free();

        //Getters:
        int getWidth();
        int getHeight();
        bool isFocusedByMouse();
        bool isFocusedByKeyboard();
        bool isMinimized();
    private:
        //La ventana en si:
        SDL_Window *window;

        //Dimensiones y propiedades:
        int width{0};
        int height{0};
        bool focusedByMouse{false};
        bool focusedByKeyboard{false};
        bool minimized{false};
        bool fullscreen{false};
};

bool Window::init() {
    window = SDL_CreateWindow("Eventos de ventana", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window != nullptr) {
        focusedByMouse = true;
        focusedByKeyboard = true;
        width = SCREEN_WIDTH;
        height = SCREEN_HEIGHT;
    }
    return window != nullptr;
}

SDL_Renderer *Window::createRenderer() {
    return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void Window::handleEvent(SDL_Event &e) {
    if(e.type == SDL_WINDOWEVENT) {
        bool updateCaption = false;
        switch(e.window.event) {
            //Cambio de tamaño:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                width = e.window.data1;
                height = e.window.data2;
                SDL_RenderPresent(renderer);
                break;
            //Se vuelve a ver:
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_RenderPresent(renderer);
                break;
            //Eventos de raton:
            case SDL_WINDOWEVENT_ENTER:
                focusedByMouse = true;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_LEAVE:
                focusedByMouse = false;
                updateCaption = true;
                break;
            //Eventos de teclado:
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                focusedByKeyboard = true;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                focusedByKeyboard = false;
                updateCaption = true;
                break;
            //Minimizado, maximizado y restaurado:
            case SDL_WINDOWEVENT_MINIMIZED:
                minimized = true;
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_RESTORED:
                minimized = false;
                break;
        }
        if(updateCaption) {
            stringstream ss;
            ss << "Eventos de ventana - MouseFocus: " << (focusedByMouse?"On":"Off") << " KeyboardFocus: " << (focusedByKeyboard?"On":"Off");
            SDL_SetWindowTitle(window, ss.str().c_str());
        }
    } else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && e.key.repeat == 0 && SDL_GetModState() & KMOD_ALT) {
        if(fullscreen) {
            SDL_SetWindowFullscreen(window, SDL_FALSE);
            fullscreen = false;
        } else {
            SDL_SetWindowFullscreen(window, SDL_TRUE);
            fullscreen = true;
            minimized = false;
        }
    }
}

void Window::free() {
    SDL_DestroyWindow(window);
}

int Window::getWidth() {
    return width;
}

int Window::getHeight() {
    return height;
}

bool Window::isFocusedByKeyboard() {
    return focusedByKeyboard;
}

bool Window::isFocusedByMouse() {
    return focusedByMouse;
}

bool Window::isMinimized() {
    return minimized;
}

Window window;
Texture sceneTexture;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        if(!window.init()) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = window.createRenderer();
            if(renderer == nullptr) {
                cout << SDL_GetError() << endl;
                success = false;
            } else {
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!sceneTexture.loadFromFile("assets/lesson35/window.png")) {
        success = false;
    }

    return success;
}

void close() {
    sceneTexture.free();
    SDL_DestroyRenderer(renderer);
    window.free();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;
            SDL_Event e;
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else {
                        window.handleEvent(e);
                    }
                }
                if(!window.isMinimized()) {
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderClear(renderer);
                    sceneTexture.render((window.getWidth() - sceneTexture.getWidth())/2, (window.getHeight() - sceneTexture.getHeight())/2);
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }
    close();
    return 0;
}
