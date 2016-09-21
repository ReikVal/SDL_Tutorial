#include <SDL.h>
#include <stdio.h>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[]) {
    //Declaramos la ventana:
    SDL_Window *window = NULL;
    //Y la superficie:
    SDL_Surface *surface = NULL;

    //Inicializamos SDL, comprobando que no haya ningún error:
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha sido inicializado correctamente: " << SDL_GetError() << endl;
    } else {
        //Hemos conseguido arrancar SDL, por lo que creamos la ventana:
        window = SDL_CreateWindow("Hola mundo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        //Comprobamos que la hemos creado correctamente:
        if(window == NULL) {
            cout << "La ventana no ha sido creada: " << SDL_GetError() << endl;
        } else {
            //Ha sido creada correctamente, por lo que creamos la superficie:
            surface = SDL_GetWindowSurface(window);
            //Y ahora creamos la magia de pintar (parece un pantallazo azul):
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x1F, 0x1F, 0xFF));
            //Update:
            SDL_UpdateWindowSurface(window);
            //Esperamos 2 segundos para después cerrar la ventana
            SDL_Delay(2000);
        }
    }

    //Destruimos la ventana en caso de haberse creado
    SDL_DestroyWindow(window);
    //Cerramos todos los subsistemas de SDL abiertos
    SDL_Quit();

    return 0;
}
