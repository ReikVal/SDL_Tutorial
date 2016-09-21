#include <SDL.h>
#include <iostream>
using namespace std;

bool init();
bool loadMedia();
void quit();

SDL_Window *w = NULL;
SDL_Surface *s = NULL;

SDL_Surface *image = NULL;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha podido iniciarse: " << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Manejando eventos", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "La ventana no ha podido crearse: " << SDL_GetError() << endl;
            success = false;
        } else {
            s = SDL_GetWindowSurface(w);
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    image = SDL_LoadBMP("assets/lesson03/x.bmp");
    if(image == NULL) {
        cout << "La imagen no ha podido ser cargada: " << SDL_GetError() << endl;
        success = false;
    }

    return success;
}

void quit() {
    SDL_FreeSurface(image);
    SDL_DestroyWindow(w);
    SDL_Quit();
}

int main(int argc, char* args[]) {

    if(init()) {
        if(loadMedia()) {
            bool quit = false;

            SDL_Event e;

            //Mientras no tengamos que salir:
            while(!quit) {
                //Miramos los eventos que hay:
                while(SDL_PollEvent(&e) != 0) {
                    //¿Esta el usuario pidiendo cerrar?
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }
                SDL_BlitSurface(image, NULL, s, NULL);

                SDL_UpdateWindowSurface(w);
            }
        }
    }

    quit();

    return 0;
}
