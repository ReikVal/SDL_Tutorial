#include <SDL.h>
#include <cstdlib>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int TOTAL_PARTICLES = 20;

SDL_Renderer *renderer;
SDL_Window *window;

class Texture {
    public:
        ~Texture();

        bool loadFromFile(string path);
        void free();
        void render(int x, int y, SDL_Rect *clip = NULL);

        void setAlpha(Uint8 alpha);

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

void Texture::render(int x, int y, SDL_Rect *clip) {
    SDL_Rect rect = {x, y, width, height};
    if(clip != nullptr) {
        rect.w = clip->w;
        rect.h = clip->h;
    }
    SDL_RenderCopy(renderer, texture, clip, &rect);
}

void Texture::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(texture, alpha);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture dotTexture;
Texture redTexture;
Texture greenTexture;
Texture blueTexture;
Texture shimmerTexture;

class Particle {
    public:
        Particle(int x, int y);
        void render();
        bool isDead();
    private:
        int x;
        int y;

        int frame;

        Texture *texture;
};

Particle::Particle(int x, int y):
    x{x - 5 + (rand()%25)}, y{y - 5 + (rand()%25)}, frame{rand()%5} {
    switch(rand()%3) {
        case 0: texture = &redTexture; break;
        case 1: texture = &greenTexture; break;
        case 2: texture = &blueTexture; break;
    }
}

void Particle::render() {
    //Renderizamos la particula:
    texture->render(x, y);

    //Renderizamos el brillo cada dos frames:
    if(frame %2 == 0) {
        shimmerTexture.render(x, y);
    }

    //Animamos la particula:
    frame++;
}

bool Particle::isDead() {
    return frame > 10;
}

class Dot {
    public:
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;

        static const int DOT_VEL = 10;

        Dot();
        ~Dot();

        void handleEvent(SDL_Event &e);

        void move();
        void render();
    private:
        int x{0};
        int y{0};

        int dx{0};
        int dy{0};

        //Le añadimos particulas:
        Particle *particles[TOTAL_PARTICLES];

        //Metodo de renderizado de particulas:
        void renderPartciles();
};

Dot::Dot() {
    for(int i = 0; i < TOTAL_PARTICLES; i++) {
        particles[i]= new Particle(x, y);
    }
}

Dot::~Dot() {
    for(int i = 0; i < TOTAL_PARTICLES; i++) {
        delete particles[i];
    }
}

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
    if(x < 0 || x + DOT_WIDTH > SCREEN_WIDTH) {
        x -= dx;
    }

    y += dy;
    if(y < 0 || y + DOT_HEIGHT > SCREEN_HEIGHT) {
        y -= dy;
    }
}

void Dot::render() {
    dotTexture.render(x, y);
    //Tambien renderizamos las particulas:
    renderPartciles();
}

void Dot::renderPartciles() {
    for(int i = 0; i < TOTAL_PARTICLES; i++) {
        //Borrado de las particulas:
        if(particles[i]->isDead()) {
            delete particles[i];
            particles[i] = new Particle(x, y);
        }
        //Renderizado de la particula:
        particles[i]->render();
    }
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Ejemplo simple de particulas", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    if(!dotTexture.loadFromFile("assets/lesson38/dot.bmp")) {
        success = false;
    }

    if(!redTexture.loadFromFile("assets/lesson38/red.bmp")) {
        success = false;
    }

    if(!greenTexture.loadFromFile("assets/lesson38/green.bmp")) {
        success = false;
    }

    if(!blueTexture.loadFromFile("assets/lesson38/blue.bmp")) {
        success = false;
    }

    if(!shimmerTexture.loadFromFile("assets/lesson38/shimmer.bmp")) {
        success = false;
    }

    redTexture.setAlpha(185);
    greenTexture.setAlpha(185);
    blueTexture.setAlpha(185);
    shimmerTexture.setAlpha(185);

    return success;
}

void close() {
    dotTexture.free();
    redTexture.free();
    greenTexture.free();
    blueTexture.free();
    shimmerTexture.free();
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
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                    dot.handleEvent(e);
                }

                dot.move();

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                dot.render();

                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}

