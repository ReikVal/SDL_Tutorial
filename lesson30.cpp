#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
using namespace std;

const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Renderer *renderer;
SDL_Window *window;


class Texture {
    public:
        ~Texture();

        bool loadFromFile(string path);
        void free();
        void render(int x, int y, SDL_Rect *clip = NULL);

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
        cout << SDL_GetError() << endl;
    } else {
        free();
        SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0, 0xFF, 0xFF));
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

void Texture::render(int x, int y, SDL_Rect *clip) {
    SDL_Rect rect = {x, y, width, height};
    if(clip != nullptr) {
        rect.w = clip->w;
        rect.h = clip->h;
    }
    SDL_RenderCopy(renderer, texture, clip, &rect);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture dotTexture;
Texture bgTexture;

class Dot {
    public:
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;

        static const int DOT_VEL = 10;

        Dot() {}

        void handleEvent(SDL_Event &e);

        void move();
        void render(int camX, int camY);

        int getX();
        int getY();
    private:
        int x{0};
        int y{0};

        int dx{0};
        int dy{0};
};

void Dot::handleEvent(SDL_Event &e) {
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
            case SDLK_UP: dy -= DOT_VEL; break;
            case SDLK_DOWN: dy += DOT_VEL; break;
            case SDLK_LEFT: dx -= DOT_VEL; break;
            case SDLK_RIGHT: dx += DOT_VEL; break;
        }
    }
    if(e.type == SDL_KEYUP) {
        switch(e.key.keysym.sym) {
            case SDLK_UP: dy += DOT_VEL; break;
            case SDLK_DOWN: dy -= DOT_VEL; break;
            case SDLK_LEFT: dx += DOT_VEL; break;
            case SDLK_RIGHT: dx -= DOT_VEL; break;
        }
    }
}

void Dot::move() {
    x += dx;
    if(x < 0 || x + DOT_WIDTH > LEVEL_WIDTH) {
        x -= dx;
    }

    y += dy;
    if(y < 0 || y + DOT_HEIGHT > LEVEL_HEIGHT) {
        y -= dy;
    }
}

void Dot::render(int camX, int camY) {
    dotTexture.render(x - camX, y - camY);
}

int Dot::getX() {
    return x;
}

int Dot::getY() {
    return y;
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Scrolling", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == nullptr) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

    if(!dotTexture.loadFromFile("assets/lesson30/dot.bmp")) {
        success = false;
    }

    if(!bgTexture.loadFromFile("assets/lesson30/bg.png")) {
        success = false;
    }

    return success;
}

void close() {
    dotTexture.free();
    bgTexture.free();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;
            SDL_Event e;
            Dot dot;
            SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                    dot.handleEvent(e);
                }

                dot.move();

                //Actualizamos la camara:
                camera.x = (dot.getX() + Dot::DOT_WIDTH/2) - SCREEN_WIDTH/2;
                camera.y = (dot.getY() + Dot::DOT_HEIGHT/2) - SCREEN_HEIGHT/2;
                if(camera.x < 0) {
                    camera.x = 0;
                } else if(camera.x + camera.w > LEVEL_WIDTH) {
                    camera.x = LEVEL_WIDTH - camera.w;
                }

                if(camera.y < 0) {
                    camera.y = 0;
                } else if(camera.y + camera.h > LEVEL_HEIGHT) {
                    camera.y = LEVEL_HEIGHT - camera.h;
                }

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                //Renderizamos la parte del fondo que se ve en la camara:
                bgTexture.render(0, 0, &camera);

                //Renderizamos el punto con la posicion relativa a la camara:
                dot.render(camera.x, camera.y);

                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
