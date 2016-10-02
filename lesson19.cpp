#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <cmath>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Para evitar que movimientos minimos del joystick haga que interactue se pone una DEAD ZONE.
const int JOYSTICK_DEAD_ZONE = 8000;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;
//Puntero al pad:
SDL_Joystick *controller = NULL;

class Texture {
    public:
        Texture();
        ~Texture();

        bool loadFromFile(string path);
        void free();
        void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

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
        width = 0;
        height = 0;
    }
}

void Texture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip) {
    SDL_Rect rect = {x, y, width, height};
    if(clip != NULL) {
        rect.w = clip->w;
        rect.h = clip->h;
    }
    SDL_RenderCopyEx(renderer, texture, clip, &rect, angle, center, flip);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture arrow;
bool init();
bool loadMedia();
void close();

bool init() {
    bool success = true;

    //Inicializamos tambien el modulo de controllers:
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        cout << "SDL no ha podido ser inicializado: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
            cout << "No escalado lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Jugando con el mando", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                    cout << "No modulo de imagenes: " << IMG_GetError() << endl;
                    success = false;
                } else if(SDL_NumJoysticks() > 0) {
                    //Comprobamos los que hay conectados:
                    for(int i = 0; i < SDL_NumJoysticks(); i++) {
                        cout << SDL_JoystickNameForIndex(i) << endl;
                    }
                    //Y seleccionamos el 0:
                    controller = SDL_JoystickOpen(0);
                    if(controller == NULL) {
                        cout << "No se ha podido abrir el controller: " << SDL_GetError() << endl;
                    }
                } else {
                    cout << "No ha ningun controller" << endl;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!arrow.loadFromFile("assets/lesson19/arrow.png")) {
        success = false;
    }

    return success;
}

void close() {
    arrow.free();
    SDL_JoystickClose(controller);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);
    IMG_Quit();
    SDL_Quit();
}

int main (int argc, char* args[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;
            SDL_Event e;
            int xDir = 0;
            int yDir = 0;
            //En el tutorial asume que la id del controller es 0, pero no es asi, no olvidar encontrar la id:
            int id = SDL_JoystickInstanceID(controller);
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_JOYAXISMOTION) {
                        //Comprobamos que estamos moviendo el stick, y miramos si es el de nuestro controller:
                        if(e.jaxis.which == id) {
                            //De ser así, ponemos xDir e yDir para posteriormente girar la flecha:
                            if(e.jaxis.axis == 0) {
                                if(e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
                                    xDir = -1;
                                } else if(e.jaxis.value > JOYSTICK_DEAD_ZONE) {
                                    xDir = 1;
                                } else {
                                    xDir = 0;
                                }
                            } else if(e.jaxis.axis == 1) {
                                if(e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
                                    yDir = -1;
                                } else if(e.jaxis.value > JOYSTICK_DEAD_ZONE) {
                                    yDir = 1;
                                } else {
                                    yDir = 0;
                                }
                            }
                        }
                    }
                }
                //Calculamos el angulo:
                double jAngle = atan2((double)yDir, (double)xDir)*(180/M_PI);
                if(xDir == 0 && yDir == 0) {
                    jAngle = 0;
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                arrow.render((SCREEN_WIDTH - arrow.getWidth())/2, (SCREEN_HEIGHT - arrow.getHeight())/2, NULL, jAngle);
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
