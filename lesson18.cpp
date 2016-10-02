#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class Texture {
    public:
        Texture();
        ~Texture();
        bool loadFromFile(string path);
        void free();
        void render(int x, int y);
    private:
        SDL_Texture *texture;
        int width;
        int height;
};

Texture upTexture;
Texture downTexture;
Texture rightTexture;
Texture leftTexture;
Texture defaultTexture;

bool init();
bool loadMedia();
void close();

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;

Texture::Texture() {
    texture = NULL;
}

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(string path) {
    SDL_Surface *surf = IMG_Load(path.c_str());
    if(surf == NULL) {
        cout << "No superficie: " << IMG_GetError() << endl;
    } else {
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
    }
}

void Texture::render(int x, int y) {
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "No SDL: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
            cout << "No escalado lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Key states", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "No ventana: " << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL) {
                cout << "No renderer: " << SDL_GetError() << endl;
                success = false;
            } else {
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << "No imagenes: " << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!rightTexture.loadFromFile("assets/lesson18/right.png")) {
        success = false;
    } else if(!leftTexture.loadFromFile("assets/lesson18/left.png")) {
        success = false;
    } else if(!upTexture.loadFromFile("assets/lesson18/up.png")) {
        success = false;
    } else if(!downTexture.loadFromFile("assets/lesson18/down.png")) {
        success = false;
    } else if(!defaultTexture.loadFromFile("assets/lesson18/press.png")) {
        success = false;
    }

    return success;
}

void close() {
    upTexture.free();
    downTexture.free();
    rightTexture.free();
    leftTexture.free();
    defaultTexture.free();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);

    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;
            SDL_Event e;
            Texture *currentTexture = NULL;
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }
                //Obtenemos el array del estado de las teclas con SDL_GetKeyboardState
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                //Y aquí las comparamos, hay que usar las constantes de SDL_SCANCODE
                if(state[SDL_SCANCODE_W]) {
                    currentTexture = &upTexture;
                } else if(state[SDL_SCANCODE_S]) {
                    currentTexture = &downTexture;
                } else if(state[SDL_SCANCODE_A]) {
                    currentTexture = &leftTexture;
                } else if(state[SDL_SCANCODE_D]) {
                    currentTexture = &rightTexture;
                } else {
                    currentTexture = &defaultTexture;
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                currentTexture->render(0, 0);
                SDL_RenderPresent(renderer);
            }
        }
    }

    close();
    return 0;
}
