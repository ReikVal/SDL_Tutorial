#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *image = NULL;

bool init();
bool loadMedia();
SDL_Texture *loadTexture(string);
void close();

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha podido ser inicializado: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "La calidad del escalado no ha podido ponerse en lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Trabajando con ViewPorts", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "No se ha podido crear la ventana: " << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
            if(renderer == NULL) {
                cout << "No se ha podido crear el renderer: " << SDL_GetError() << endl;
                success = false;
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    image = loadTexture("assets/lesson09/viewport.png");
    if(image == NULL) {
        success = false;
    }

    return success;
}

SDL_Texture *loadTexture(string path) {
    SDL_Texture *texture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL) {
        cout << "No se ha podido cargar la imagen " << path << ": " << IMG_GetError() << endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    }
    SDL_FreeSurface(loadedSurface);
    return texture;
}

void close() {
    SDL_DestroyTexture(image);
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
                    }
                }
                //Limpiamos la pantalla:
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                //Creamos el ViewPort de arriba izquierda a partir de un rectangulo:
                SDL_Rect topLeftVP = {0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
                SDL_RenderSetViewport(renderer, &topLeftVP);
                //Y renderizamos la textura:
                SDL_RenderCopy(renderer, image, NULL, NULL);

                //De igual forma creamos el de arriba derecha y el de abajo:
                SDL_Rect topRightVP = {SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
                SDL_RenderSetViewport(renderer, &topRightVP);
                SDL_RenderCopy(renderer, image, NULL, NULL);
                SDL_Rect botVP = {0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2};
                SDL_RenderSetViewport(renderer, &botVP);
                SDL_RenderCopy(renderer, image, NULL, NULL);

                //Finalmente, presentamos pantalla:
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
