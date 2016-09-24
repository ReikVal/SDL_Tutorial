#include <SDL.h>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;

bool init();
void close();

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha podido ser inicializado: " << SDL_GetError() << endl;
        success = false;
    } else {
        //Para que el escalado sea lineal, usamos lo siguiente:
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "El render scale no ha podido ser definido como lineal: " << SDL_GetError() << endl;
        }

        w = SDL_CreateWindow("Geometria 101", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "La ventana no ha podido ser creada: " << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
            if(renderer == NULL) {
                cout << "El renderer no ha sido creado: " << SDL_GetError() << endl;
                success = false;
            }
        }
    }

    return success;
}

void close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if(init()) {
        bool quit = false;
        SDL_Event e;
        while(!quit) {
            while(SDL_PollEvent(&e) != 0) {
                if(e.type == SDL_QUIT) {
                    quit = true;
                }
            }
            //Limpiamos la pantalla:
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);
            //Creamos un rectangulo y lo rellenamos de rojo:
            SDL_Rect fillRect = { SCREEN_WIDTH/4 , SCREEN_HEIGHT/4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_RenderFillRect(renderer, &fillRect);
            //Creamos un rectangulo azul:
            SDL_Rect outlineRect = { SCREEN_WIDTH/6, SCREEN_HEIGHT/6, SCREEN_WIDTH*2/3, SCREEN_HEIGHT*2/3 };
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
            SDL_RenderDrawRect(renderer, &outlineRect);
            //Creamos una linea horizontal verde en el centro:
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2);
            //Creamos puntos discontinuos verticalmente en el centro:
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
            for(int i = 0; i < SCREEN_HEIGHT; i+=4) {
                SDL_RenderDrawPoint(renderer, SCREEN_WIDTH/2, i);
            }
            //Presentamos lo que hicimos:
            SDL_RenderPresent(renderer);
        }
    }
    close();
    return 0;
}
