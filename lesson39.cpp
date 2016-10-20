#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

//Las propiedades de los tiles:
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

//Los tipos de tiles:
const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    bool collision = false;
    SDL_Rect result;
    if(SDL_IntersectRect(&a, &b, &result) == SDL_TRUE) {
        collision = true;
    }
    return collision;
}

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
Texture tileTexture;
SDL_Rect tileClips[TOTAL_TILE_SPRITES];

//Clase que representa el tile:
class Tile {
    public:
        Tile(int x, int y, int type): box{x, y, TILE_WIDTH, TILE_HEIGHT}, type(type) {}
        void render(SDL_Rect &camera);
        int getType();
        SDL_Rect getBox();
    private:
        SDL_Rect box;
        int type;
};

void Tile::render(SDL_Rect &camera) {
    if(checkCollision(camera, box)) {
        tileTexture.render(box.x - camera.x, box.y - camera.y, &tileClips[type]);
    }
}

int Tile::getType() {
    return type;
}

SDL_Rect Tile::getBox() {
    return box;
}

bool touchesWall(SDL_Rect box, Tile *tiles[]) {
    for(int i = 0; i < TOTAL_TILES; i++) {
        if(tiles[i]->getType() >= TILE_CENTER && tiles[i]->getType() <= TILE_TOPLEFT) {
            if(checkCollision(box, tiles[i]->getBox())) {
                return true;
            }
        }
    }
    return false;
}

class Dot {
    public:
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;
        static const int DOT_VEL = 10;

        void handleEvent(SDL_Event &e);
        void move(Tile *tiles[]);
        void setCamera(SDL_Rect &camera);
        void render(SDL_Rect &camera);
    private:
        SDL_Rect box{0, 0, DOT_WIDTH, DOT_HEIGHT};
        int vx{0};
        int vy{0};
};

void Dot::handleEvent(SDL_Event &e) {
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
            case SDLK_UP: vy -= DOT_VEL; break;
            case SDLK_DOWN: vy += DOT_VEL; break;
            case SDLK_LEFT: vx -= DOT_VEL; break;
            case SDLK_RIGHT: vx += DOT_VEL; break;
        }
    } else if(e.type == SDL_KEYUP) {
        switch(e.key.keysym.sym) {
            case SDLK_UP: vy += DOT_VEL; break;
            case SDLK_DOWN: vy -= DOT_VEL; break;
            case SDLK_LEFT: vx += DOT_VEL; break;
            case SDLK_RIGHT: vx -= DOT_VEL; break;
        }
    }
}

void Dot::move(Tile *tiles[]) {
    box.x += vx;

    if(box.x < 0 || box.x + DOT_WIDTH > LEVEL_WIDTH || touchesWall(box, tiles)) {
        box.x -= vx;
    }

    box.y += vy;

    if(box.y < 0 || box.y + DOT_HEIGHT > LEVEL_HEIGHT || touchesWall(box, tiles)) {
        box.y -= vy;
    }
}

void Dot::setCamera(SDL_Rect &camera) {
    camera.x = (box.x + DOT_WIDTH/2) - SCREEN_WIDTH/2;
    camera.y = (box.y + DOT_HEIGHT/2) - SCREEN_HEIGHT/2;

    if(camera.x < 0) {
        camera.x = 0;
    } else if(camera.x > LEVEL_WIDTH - camera.w) {
        camera.x = LEVEL_WIDTH - camera.w;
    }

    if(camera.y < 0) {
        camera.y = 0;
    } else if(camera.y > LEVEL_HEIGHT - camera.h) {
        camera.y = LEVEL_HEIGHT - camera.h;
    }
}

void Dot::render(SDL_Rect &camera) {
    dotTexture.render(box.x - camera.x, box.y - camera.y);
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Tiles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

bool setTiles(Tile *tiles[]) {
    bool success = true;

    int x = 0;
    int y = 0;

    std::ifstream map("assets/lesson39/lazy.map");

    if(map == nullptr) {
        cout << "No se ha podido cargar el mapa" << endl;
        success = false;
    } else {
        for(int i = 0; i < TOTAL_TILES; i++) {
            int tileType = -1;

            //Leemos el tile:
            map >> tileType;
            if(map.fail()) {
                cout << "Error cargando el mapa" << endl;
                break; //Se deberia usar mejor un throw
            }

            //Si el numero es correcto:
            if(tileType >= 0 && tileType <= TOTAL_TILE_SPRITES) {
                tiles[i] = new Tile(x, y, tileType);
            } else {
                cout << "Tile no valido" << endl;
                break;
            }
            x += TILE_WIDTH;
            if(x >= LEVEL_WIDTH) {
                x = 0;
                y += TILE_HEIGHT;
            }
        }

        if(success) {
            tileClips[TILE_RED].x = 0;
            tileClips[TILE_RED].y = 0;
            tileClips[TILE_RED].w = TILE_WIDTH;
            tileClips[TILE_RED].h = TILE_HEIGHT;

            tileClips[TILE_GREEN].x = 0;
            tileClips[TILE_GREEN].y = 80;
            tileClips[TILE_GREEN].w = TILE_WIDTH;
            tileClips[TILE_GREEN].h = TILE_HEIGHT;

            tileClips[TILE_BLUE].x = 0;
            tileClips[TILE_BLUE].y = 160;
            tileClips[TILE_BLUE].w = TILE_WIDTH;
            tileClips[TILE_BLUE].h = TILE_HEIGHT;

            tileClips[TILE_TOPLEFT].x = 80;
            tileClips[TILE_TOPLEFT].y = 0;
            tileClips[TILE_TOPLEFT].w = TILE_WIDTH;
            tileClips[TILE_TOPLEFT].h = TILE_HEIGHT;

            tileClips[TILE_LEFT].x = 80;
            tileClips[TILE_LEFT].y = 80;
            tileClips[TILE_LEFT].w = TILE_WIDTH;
            tileClips[TILE_LEFT].h = TILE_HEIGHT;

            tileClips[TILE_BOTTOMLEFT].x = 80;
            tileClips[TILE_BOTTOMLEFT].y = 160;
            tileClips[TILE_BOTTOMLEFT].w = TILE_WIDTH;
            tileClips[TILE_BOTTOMLEFT].h = TILE_HEIGHT;

            tileClips[TILE_TOP].x = 160;
            tileClips[TILE_TOP].y = 0;
            tileClips[TILE_TOP].w = TILE_WIDTH;
            tileClips[TILE_TOP].h = TILE_HEIGHT;

            tileClips[TILE_CENTER].x = 160;
            tileClips[TILE_CENTER].y = 80;
            tileClips[TILE_CENTER].w = TILE_WIDTH;
            tileClips[TILE_CENTER].h = TILE_HEIGHT;

            tileClips[TILE_BOTTOM].x = 160;
            tileClips[TILE_BOTTOM].y = 160;
            tileClips[TILE_BOTTOM].w = TILE_WIDTH;
            tileClips[TILE_BOTTOM].h = TILE_HEIGHT;

            tileClips[TILE_TOPRIGHT].x = 240;
            tileClips[TILE_TOPRIGHT].y = 0;
            tileClips[TILE_TOPRIGHT].w = TILE_WIDTH;
            tileClips[TILE_TOPRIGHT].h = TILE_HEIGHT;

            tileClips[TILE_RIGHT].x = 240;
            tileClips[TILE_RIGHT].y = 80;
            tileClips[TILE_RIGHT].w = TILE_WIDTH;
            tileClips[TILE_RIGHT].h = TILE_HEIGHT;

            tileClips[TILE_BOTTOMRIGHT].x = 240;
            tileClips[TILE_BOTTOMRIGHT].y = 160;
            tileClips[TILE_BOTTOMRIGHT].w = TILE_WIDTH;
            tileClips[TILE_BOTTOMRIGHT].h = TILE_HEIGHT;
        }
    }

    map.close();

    return success;
}

bool loadMedia(Tile *tiles[]) {
    bool success = true;
    if(!dotTexture.loadFromFile("assets/lesson39/dot.bmp")) {
        success = false;
    }

    if(!tileTexture.loadFromFile("assets/lesson39/tiles.png")) {
        success = false;
    }

    if(!setTiles(tiles)) {
        success = false;
    }

    return success;
}

void close(Tile *tiles[]) {
    for(int i = 0; i < TOTAL_TILES; i++) {
        if(tiles[i] != nullptr) {
            delete tiles[i];
        }
    }
    dotTexture.free();
    tileTexture.free();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Tile *tiles[TOTAL_TILES];
    if(init()) {
        if(loadMedia(tiles)) {
            bool quit = false;
            SDL_Event e;
            Dot dot;
            SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            while(!quit) {
                while(SDL_PollEvent(&e)) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                    dot.handleEvent(e);
                }
                //Movemos el punto y la camara:
                dot.move(tiles);
                dot.setCamera(camera);
                //Limpiamos screen:
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                //Renderizamos el mapa:
                for(int i = 0; i < TOTAL_TILES; i++) {
                    tiles[i]->render(camera);
                }
                //Renderizamos el punto:
                dot.render(camera);

                SDL_RenderPresent(renderer);
            }
        }
    }
    close(tiles);
    return 0;
}
