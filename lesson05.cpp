#include <SDL.h>
#include <iostream>
#include <string>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();

SDL_Window *w = NULL;
SDL_Surface *s = NULL;

SDL_Surface *loadSurface(string);

SDL_Surface *surfaceToStrecth = NULL;

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

    surfaceToStrecth = loadSurface("assets/lesson05/stretch.bmp");
    if(surfaceToStrecth == NULL) {
        cout << "Error al cargar y optimizar la superficie: " << SDL_GetError() << endl;
        success = false;
    }

    return success;
}

void close() {
    SDL_FreeSurface(surfaceToStrecth);
    SDL_DestroyWindow(w);
    SDL_Quit();
}

SDL_Surface *loadSurface(string path) {
    //Declaramos la superficie que vamos a devolver:
    SDL_Surface *optimizedSurface = NULL;

    //Cargamos la superficie a partir del path
    SDL_Surface *surface = SDL_LoadBMP(path.c_str());

    if(surface != NULL) {
        optimizedSurface = SDL_ConvertSurface(surface, s->format, 0);
    }
    //Limpiamos la superficie anterior, ya que tenemos una optimizada
    SDL_FreeSurface(surface);

    return optimizedSurface;
}

int main(int argc, char *args[]) {
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
                //Vamos a crear un rectangulo para después hacer un escalado a la imagen:
                SDL_Rect stretchRect;
                stretchRect.x = 0;
                stretchRect.y = 0;
                stretchRect.w = SCREEN_WIDTH;
                stretchRect.h = SCREEN_HEIGHT;

                //Y ahora hacemos el escalado de la imagen para que ocupe toda la pantalla y el blit:
                SDL_BlitScaled(surfaceToStrecth, NULL, s, &stretchRect);

                //Y por último hacemos update de la ventana como siempre:
                SDL_UpdateWindowSurface(w);
            }
        }
    }
    close();
    return 0;
}
