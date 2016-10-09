#include <SDL.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *window;
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
    SDL_Surface *surf = SDL_LoadBMP(path.c_str());
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

Texture dotTexture;

bool checkCollision(vector<SDL_Rect> &a, vector<SDL_Rect> &b) {
    SDL_Rect rect;
    for(const auto v: a) {
        for(const auto w: b) {
            //Hay que pasarle una estructura SDL_Rect en el tercer parametro o siempre devolvera SDL_FALSE;
            if(SDL_IntersectRect(&v, &w, &rect)) {
                return true;
            }
        }
    }
    return false;
}

class Dot {
    public:
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIHGT = 20;

        static const int DOT_VEL = 1;

        Dot(int x, int y);

        void handleEvent(SDL_Event &e);
        //Metodo para moverse mirando si choca contra otras cajas de colision:
        void move(vector<SDL_Rect> &otherColliders);
        void render();
        vector<SDL_Rect> &getColliders();
    private:
        int x;
        int y;
        int dx{0};
        int dy{0};
        //Las cajas de colision:
        vector<SDL_Rect> colliders;
        //Metodo privado para mover las cajas de colision:
        void shiftColliders();
};

Dot::Dot(int x, int y):
    x(x), y(y) {
    //Creamos las cajas necesarias:
    colliders.resize(11);
    //Y las inicializamos:
    colliders[0].w = 6;
    colliders[0].h = 1;
    colliders[1].w = 10;
    colliders[1].h = 1;
    colliders[2].w = 14;
    colliders[2].h = 1;
    colliders[3].w = 16;
    colliders[3].h = 2;
    colliders[4].w = 18;
    colliders[4].h = 2;
    colliders[5].w = 20;
    colliders[5].h = 6;
    colliders[6].w = 18;
    colliders[6].h = 2;
    colliders[7].w = 16;
    colliders[7].h = 2;
    colliders[8].w = 14;
    colliders[8].h = 1;
    colliders[9].w = 10;
    colliders[9].h = 1;
    colliders[10].w = 6;
    colliders[10].h = 1;
    //Y las ponemos en su posicion:
    shiftColliders();
}

void Dot::handleEvent(SDL_Event &e) {
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
            case SDLK_UP: dy -= DOT_VEL; break;
            case SDLK_DOWN: dy += DOT_VEL; break;
            case SDLK_LEFT: dx -= DOT_VEL; break;
            case SDLK_RIGHT: dx += DOT_VEL; break;
        }
    } else if(e.type == SDL_KEYUP) {
        switch(e.key.keysym.sym) {
            case SDLK_UP: dy += DOT_VEL; break;
            case SDLK_DOWN: dy -= DOT_VEL; break;
            case SDLK_LEFT: dx += DOT_VEL; break;
            case SDLK_RIGHT: dx -= DOT_VEL; break;
        }
    }
}

void Dot::move(vector<SDL_Rect> &otherColliders) {
    x += dx;
    shiftColliders();
    //Si choca deshacemos el cambio:
    if(x < 0 || x + DOT_WIDTH > SCREEN_WIDTH || checkCollision(colliders, otherColliders)) {
        x -= dx;
        shiftColliders();
    }

    y += dy;
    shiftColliders();
    //Si choca deshacemos el cambio:
    if(y < 0 || y + DOT_HEIHGT > SCREEN_HEIGHT || checkCollision(colliders, otherColliders)) {
        y -= dy;
        shiftColliders();
    }
}

void Dot::shiftColliders() {
    int r = 0;
    for(auto &v: colliders) {
        v.x = x + (DOT_WIDTH - v.w)/2;
        v.y = y + r;
        r += v.h;
    }
}

vector<SDL_Rect> &Dot::getColliders() {
    return colliders;
}

void Dot::render() {
    dotTexture.render(x, y);
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Pixel-Perfect collision", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == nullptr) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == nullptr) {
                cout << SDL_GetError() << endl;
                success = false;
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!dotTexture.loadFromFile("assets/lesson28/dot.bmp")) {
        success = false;
    }

    return success;
}

void close() {
    dotTexture.free();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;
            SDL_Event e;
            Dot dot(0, 0);
            Dot otherDot(SCREEN_WIDTH/4, SCREEN_HEIGHT/4);
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                    dot.handleEvent(e);
                }
                dot.move(otherDot.getColliders());
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                dot.render();
                otherDot.render();
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
