#include <SDL.h>
#include <stdio.h>
#include <iostream>
using namespace std;

//Definimos las funciones que vamos a hacer, para modularizar un poco la inicialización y ejecución de SDL:
bool init();
bool loadMedia();
void close();

SDL_Window *w = NULL;
SDL_Surface *screen = NULL;

//Tambien declaramos la imagen que vamos a mostrar en pantalla:
SDL_Surface *image = NULL;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha sido inicializado correctamente: " << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Leccion 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "La ventana no ha sido creada: " << SDL_GetError() << endl;
            success = false;
        } else {
            screen = SDL_GetWindowSurface(w);
        }
    }

    return success;
}

bool loadMedia() {
    bool success;

    //Cargando la imagen:
    image = SDL_LoadBMP("assets/lesson02/hello_world.bmp");
    if(image == NULL) {
        cout << "No se ha podido cargar la imagen: " << SDL_GetError() << endl;
        success = false;
    }

    return success;
}

void close() {
    //Destruimos las superficies que hemos creado:
    SDL_FreeSurface(image);
    //Destruimos la ventana (esta libera la superficie adjunta):
    SDL_DestroyWindow(w);
    //Cerramos los subsistemas:
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if(init()) {
        if(loadMedia()) {
            //Ponemos la imagen:
            SDL_BlitSurface(image, NULL, screen, NULL);
            //Y hacemos update:
            SDL_UpdateWindowSurface(w);
            //Esperamos 2 segundos para cerrar:
            SDL_Delay(2000);
        }
    }

    close();
    return 0;
}
