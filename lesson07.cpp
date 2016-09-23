#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *w = NULL;

SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

SDL_Texture *loadTexture(string);
bool init();
bool loadMedia();
void close();

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "No se ha podido inicializar SDL: " << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Trabajando con texturas", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "No se ha podido crear la ventana: " << SDL_GetError() << endl;
            success = false;
        } else {
            //Creamos el renderer, el -1 significa el primer driver que soporte las flags que introducimos, y el tercer parametro es la flag.
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
            if(renderer == NULL) {
                cout << "El renderer no ha podido ser creado: " << SDL_GetError() << endl;
                success = false;
            } else {
                //Inicializamos el color del renderer
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                //E inicializamos SDL2_image para PNGs:
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << "Se ha producido un error al cargar el soporte para PNG: " << SDL_GetError() << endl;
                    success = false;
                }
            }
        }
    }
    return success;
}

SDL_Texture *loadTexture(string path) {
    SDL_Texture *newTexture = NULL;
    //Cargamos la imagen en una superficie:
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL) {
        cout << "La imagen no ha podido ser cargada: " << IMG_GetError() << endl;
    } else {
        //Creamos la textura desde la superficie:
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(newTexture == NULL) {
            cout << "No se ha podido crear la textura: " << endl;
        }
    }
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

bool loadMedia() {
    bool success = true;

    texture = loadTexture("assets/lesson07/texture.png");

    if(texture == NULL) {
        success = false;
    }

    return success;
}

void close() {
    //Destruimos la superficie y el renderer, y liberamos el resto de recursos como siempre:
    SDL_DestroyTexture(texture);
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
                while(SDL_PollEvent(&e)) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }
                //Borramos la pantalla:
                SDL_RenderClear(renderer);
                //Renderizamos la textura:
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                //Hacemos update de la screen, o la presentamos:
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
