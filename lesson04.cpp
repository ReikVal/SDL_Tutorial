#include <SDL.h>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Usamos un enumerado para listar las superficies que hay.
enum KeyPressSurfaceType {
    defaultSurface,
    upSurface,
    downSurface,
    leftSurface,
    rightSurface,
    totalSurfaces
    //Buscar una altenativa al enumerado más adelante. El enum class de C++11 no funciona directamente para indices en arrays.
};

bool init();
bool loadMedia();
void close();

SDL_Window *w = NULL;
SDL_Surface *s = NULL;

//Declaramos la superficie actual:
SDL_Surface *currentSurface = NULL;

//Y declaramos dichas superficies:
SDL_Surface *keyPressSurfaces[KeyPressSurfaceType::totalSurfaces];

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha podido ser inicializado: " << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Eventos con el teclado", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "La ventana no ha podido ser creada: " << SDL_GetError() << endl;
            success = false;
        } else {
            s = SDL_GetWindowSurface(w);
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    keyPressSurfaces[KeyPressSurfaceType::defaultSurface] = SDL_LoadBMP("assets/lesson04/press.bmp");
    keyPressSurfaces[KeyPressSurfaceType::upSurface] = SDL_LoadBMP("assets/lesson04/up.bmp");
    keyPressSurfaces[KeyPressSurfaceType::downSurface] = SDL_LoadBMP("assets/lesson04/down.bmp");
    keyPressSurfaces[KeyPressSurfaceType::leftSurface] = SDL_LoadBMP("assets/lesson04/left.bmp");
    keyPressSurfaces[KeyPressSurfaceType::rightSurface] = SDL_LoadBMP("assets/lesson04/right.bmp");

    if(keyPressSurfaces[KeyPressSurfaceType::defaultSurface] == NULL ||
       keyPressSurfaces[KeyPressSurfaceType::upSurface] == NULL ||
       keyPressSurfaces[KeyPressSurfaceType::downSurface] == NULL ||
       keyPressSurfaces[KeyPressSurfaceType::leftSurface] == NULL ||
       keyPressSurfaces[KeyPressSurfaceType::rightSurface] == NULL) {
        cout << "Error cargando alguna de las imagenes: " << SDL_GetError() << endl;
        success = false;
    }

    return success;
}

void close() {
    //Liberamos las superficies con las que hemos trabajado:
    for(int i = 0; i < KeyPressSurfaceType::totalSurfaces; i++) {
        SDL_FreeSurface(keyPressSurfaces[i]);
    }

    //Destruimos la ventana y cerramos:
    SDL_DestroyWindow(w);
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;

            SDL_Event e;

            currentSurface = keyPressSurfaces[KeyPressSurfaceType::defaultSurface];

            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_KEYDOWN) {
                        //Ahora que hemos visto que es un evento de key down, comprobamos nuestras teclas:
                        switch(e.key.keysym.sym) {
                            case SDLK_UP:
                                currentSurface = keyPressSurfaces[KeyPressSurfaceType::upSurface];
                                break;
                            case SDLK_DOWN:
                                currentSurface = keyPressSurfaces[KeyPressSurfaceType::downSurface];
                                break;
                            case SDLK_LEFT:
                                currentSurface = keyPressSurfaces[KeyPressSurfaceType::leftSurface];
                                break;
                            case SDLK_RIGHT:
                                currentSurface = keyPressSurfaces[KeyPressSurfaceType::rightSurface];
                                break;
                            case SDLK_ESCAPE:
                                quit = true;
                                break;
                            default:
                                currentSurface = keyPressSurfaces[KeyPressSurfaceType::defaultSurface];
                                break;
                        }
                    }
                }
                SDL_BlitSurface(currentSurface, NULL, s, NULL);
                SDL_UpdateWindowSurface(w);
            }
        }
    }
    close();
    return 0;
}
