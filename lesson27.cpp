#include <SDL.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;

class Texture {
    public:
        Texture();
        ~Texture();
        bool loadFromFile(string path);
        void free();
        void render(int x, int y);
        int getWidth();
        int getHeight();
    private:
        SDL_Texture *texture;
        int width;
        int height;
};

Texture::Texture() {
    texture = NULL;
    width = 0;
    height = 0;
}

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(string path) {
    SDL_Surface *surf = SDL_LoadBMP(path.c_str());
    if(surf == NULL) {
        cout << "No superficie: " << SDL_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        if(texture == NULL) {
            cout << "No textura: " << SDL_GetError() << endl;
        } else {
            width = surf->w;
            height = surf->h;
        }
        SDL_FreeSurface(surf);
    }
    return texture != NULL;
}

void Texture::free() {
    if(texture != NULL) {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

void Texture::render(int x, int y) {
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture dotTexture;

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    bool collide = true;

    if(a.x + a.w < b.x) {
        collide = false;
    } else if(a.x > b.x + b.w) {
        collide = false;
    } else if(a.y + a.h < b.y) {
        collide = false;
    } else if(a.y > b.y + b.h) {
        collide = false;
    }

    return collide;
}

//Creamos una clase para el punto:
class Dot {
    private:
        //Se prefiere in-class initializers para inicializadores constantes:
        int x{0};
        int y{0};
        int vx{0};
        int vy{0};
        SDL_Rect collisionBox;
    public:
        //Velocidad y tama�o de todos los puntos:
        static const int VEL = 10;
        static const int WIDTH = 20;
        static const int HEIGHT = 20;

        //Constructor
        Dot();

        //Handle de eventos:
        void handleEvent(SDL_Event &e);

        //Metodos para mover y renderizar el punto:
        void move(SDL_Rect &wall);
        void render();
};

Dot::Dot() {
    collisionBox.w = WIDTH;
    collisionBox.h = HEIGHT;
}

void Dot::handleEvent(SDL_Event &e) {
    //e.key.repeat == 0 hace que solo sea la primera pulsaci�n
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        //Incrementamos la velocidad:
        switch(e.key.keysym.sym) {
            case SDLK_UP:
                vy -= VEL;
                break;
            case SDLK_DOWN:
                vy += VEL;
                break;
            case SDLK_LEFT:
                vx -= VEL;
                break;
            case SDLK_RIGHT:
                vx += VEL;
                break;
        }
    }
    if(e.type == SDL_KEYUP) {
        //Deshacemos el cambio de velocidad:
        switch(e.key.keysym.sym) {
            case SDLK_UP:
                vy += VEL;
                break;
            case SDLK_DOWN:
                vy -= VEL;
                break;
            case SDLK_LEFT:
                vx += VEL;
                break;
            case SDLK_RIGHT:
                vx -= VEL;
                break;
        }
    }
}

//Movemos el punto gestionando los bordes de pantalla:
void Dot::move(SDL_Rect &wall) {
    x += vx;
    collisionBox.x = x;
    if(x < 0) {
        x = 0;
    } else if(x > SCREEN_WIDTH - WIDTH) {
        x = SCREEN_WIDTH - WIDTH;
    } else if(checkCollision(collisionBox, wall)) {
        if(vx > 0 && x + WIDTH - vx <= wall.x) {
            x = wall.x - WIDTH;
        } else if(vx < 0 && x - vx >= wall.x + wall.w) {
            x = wall.x + wall.w;
        }
    }
    y += vy;
    collisionBox.y = y;
    if(y < 0) {
        y = 0;
    } else if(y > SCREEN_HEIGHT - HEIGHT) {
        y = SCREEN_HEIGHT - HEIGHT;
    } else if(checkCollision(collisionBox, wall)) {
        if(vy > 0 && y + HEIGHT - vy <= wall.y) {
            y = wall.y - HEIGHT;
        } else if(vy < 0 && y - vy >= wall.y + wall.h) {
            y = wall.y + wall.h;
        }
    }

    //Actualizamos tambien la caja de colision:
    collisionBox.x = x;
    collisionBox.y = y;
}

//Renderizamos la textura en la posicion del punto:
void Dot::render() {
    dotTexture.render(x, y);
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Colisiones con cuadrados", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL) {
                cout << SDL_GetError() << endl;
                success = false;
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!dotTexture.loadFromFile("assets/lesson27/dot.bmp")) {
        success = false;
    }

    return success;
}

void close() {
    dotTexture.free();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;
            SDL_Event e;
            Dot dot;
            //Ponemos una pared en medio:
            SDL_Rect wall = {300, 40, 40, 400};
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                    dot.handleEvent(e);
                }

                //Update despues de Handle input y antes del render:
                dot.move(wall);

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                //Pintamos la pared:
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
                SDL_RenderDrawRect(renderer, &wall);
                //Pintamos el punto:
                dot.render();
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}

