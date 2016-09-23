#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();

SDL_Surface *loadSurface(string);

SDL_Window *w = NULL;
SDL_Surface *s = NULL;
SDL_Surface *pngSurface = NULL;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha podido ser iniciado: " << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Introduciendo PNGs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "La ventana no pudo ser creada: " << SDL_GetError() << endl;
            success = false;
        } else {
            //Ponemos las flags de los tipos de imagenes que queremos abrir separadas de |
            int flags = IMG_INIT_PNG;
            //E inicializamos el modulo:
            if((IMG_Init(flags) & flags) != flags) {
                cout << "No se ha podido iniciar la manipulación de PNG: " << IMG_GetError() << endl;
                success = false;
            } else {
                s = SDL_GetWindowSurface(w);
            }
        }
    }

    return success;
}

SDL_Surface *loadSurface(string path) {
    SDL_Surface *optimizedSurface = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());

    if(loadedSurface == NULL) {
        cout << "No se ha podido cargar la imagen " << path << " : " << IMG_GetError() << endl;
    } else {
        optimizedSurface = SDL_ConvertSurface(loadedSurface, s->format, 0);
        if(optimizedSurface == NULL) {
            cout << "No se ha podido convertir la superficie" << SDL_GetError() << endl;
        }
        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}

bool loadMedia() {
    bool success = true;

    pngSurface = loadSurface("assets/lesson06/loaded.png");
    if(pngSurface == NULL) {
        success = false;
    }

    return success;
}

void close() {
    SDL_FreeSurface(pngSurface);
    SDL_DestroyWindow(w);
    //Salimos del modulo IMG
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
                    }
                }
                SDL_BlitSurface(pngSurface, NULL, s, NULL);
                SDL_UpdateWindowSurface(w);
            }
        }
    }
    close();
    return 0;
}
