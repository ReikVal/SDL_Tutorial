#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;
SDL_Joystick *controller = NULL;
SDL_Haptic *haptic = NULL;

class Texture {
    public:
        Texture();
        ~Texture();
        bool loadFromFile(string path);
        void free();
        void render(int x, int y);
    private:
        SDL_Texture *texture;
};

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
        free();
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        if(texture == NULL) {
            cout << "No textura: " << SDL_GetError() << endl;
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
    SDL_Rect rect = {x, y, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

Texture pantalla;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0) {
        cout << "No se ha inicializado SDL: " << SDL_GetError() << endl;
        success = false;
    } else {
        //No pongo escalado lineal por centrarme en lo importante.
        w = SDL_CreateWindow("Vibracion en el mando", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                    cout << "No modulo de imagenes: " << SDL_GetError() << endl;
                    success = false;
                } else if(SDL_NumJoysticks() > 0) {
                    for(int i = 0; i < SDL_NumJoysticks(); i++) {
                        cout << SDL_JoystickNameForIndex(i) << endl;
                    }
                    controller = SDL_JoystickOpen(0);
                    if(controller == NULL) {
                        cout << "No se ha podido abrir el controller: " << SDL_GetError() << endl;
                    } else {
                        //Debemos abrir el haptic e inicializar su modulo de vibracion:
                        haptic = SDL_HapticOpenFromJoystick(controller);
                        if(haptic == NULL || SDL_HapticRumbleInit(haptic) < 0) {
                            cout << "No haptic para el controlador: " << SDL_GetError() << endl;
                        }
                    }
                } else {
                    cout << "No hay ningún controller" << endl;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!pantalla.loadFromFile("assets/lesson20/splash.png")) {
        success = false;
    }

    return success;
}

void close() {
    pantalla.free();
    //Por supuesto, debemos liberar los recursos del haptic:
    SDL_HapticClose(haptic);
    SDL_JoystickClose(controller);
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
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_JOYBUTTONDOWN) {
                        //Aqui hacemos que vibre el mando con una fuerza de 0.8 durante 500ms.
                        if(SDL_HapticRumblePlay(haptic, 0.8, 500) != 0) {
                            cout << "No se ha podido iniciar el rumble: " << SDL_GetError() << endl;
                        }
                    }
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                pantalla.render(0,0);
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
