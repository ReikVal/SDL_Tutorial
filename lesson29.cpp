#include <SDL.h>
#include <string>
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

struct Circle {
    int x, y;
    int r;
};

bool checkCollision(Circle &c, SDL_Rect &r) {
    int closeX, closeY;

    if(c.x < r.x) {
        closeX = r.x;
    } else if(c.x > r.x + r.w) {
        closeX = r.x + r.w;
    } else {
        closeX = c.x;
    }

    if(c.y < r.y) {
        closeY = r.y;
    } else if(c.y > r.y + r.h) {
        closeY = r.y + r.h;
    } else {
        closeY = c.y;
    }

    if((c.x - closeX)*(c.x - closeX) + (c.y - closeY)*(c.y - closeY) < c.r*c.r) {
        return true;
    }

    return false;
}

bool checkCollision(Circle &c1, Circle &c2) {
    int totalRadiusSquared = c1.r + c2.r;
    totalRadiusSquared *= totalRadiusSquared;

    if((c1.x - c2.x)*(c1.x - c2.x) + (c1.y - c2.y)*(c1.y - c2.y) < totalRadiusSquared) {
        return true;
    }

    return false;
}

class Dot {
    public:
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;

        static const int DOT_VEL = 1;

        Dot(int x, int y);

        void handleEvent(SDL_Event &e);

        void move(SDL_Rect &square, Circle &circle);

        void render();

        Circle &getCollider();
    private:
        int x;
        int y;
        int dx{0};
        int dy{0};
        Circle collider;
        void shiftCollider();

};

Dot::Dot(int x, int y):
    x(x), y(y) {
    collider.x = x;
    collider.y = y;
    collider.r = DOT_WIDTH/2;
    shiftCollider();
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

void Dot::move(SDL_Rect &square, Circle &circle) {
    x += dx;
    shiftCollider();
    if(x - collider.r < 0 || x + collider.r > SCREEN_WIDTH || checkCollision(collider, square) || checkCollision(collider, circle)) {
        x -= dx;
        shiftCollider();
    }

    y += dy;
    shiftCollider();
    if(y - collider.r < 0 || y + collider.r > SCREEN_HEIGHT || checkCollision(collider, square) || checkCollision(collider, circle)) {
        y -= dy;
        shiftCollider();
    }
}

void Dot::render() {
    dotTexture.render(x - collider.r, y - collider.r);
}

Circle &Dot::getCollider() {
    return collider;
}

void Dot::shiftCollider() {
    collider.x = x;
    collider.y = y;
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Circular collision", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    if(!dotTexture.loadFromFile("assets/lesson29/dot.bmp")) {
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
            Dot dot(Dot::DOT_WIDTH/2, Dot::DOT_WIDTH/2);
            Dot otherDot(SCREEN_WIDTH/4, SCREEN_HEIGHT/4);
            SDL_Rect wall = {300, 40, 40, 400};
            while(!quit) {
                while(SDL_PollEvent(&e)) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                    dot.handleEvent(e);
                }

                dot.move(wall, otherDot.getCollider());

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
                SDL_RenderDrawRect(renderer, &wall);
                dot.render();
                otherDot.render();
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
