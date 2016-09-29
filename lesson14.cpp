#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;

bool init();
bool loadMedia();
void close();

class Texture {
    public:
        Texture();
        ~Texture();

        bool loadFromFile(string path);
        void free();
        void render(int x, int y, SDL_Rect *clip = NULL);

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
    SDL_Surface *surface = IMG_Load(path.c_str());
    if(surface == NULL) {
        cout << "No se ha podido cargar la superficie: " << IMG_GetError() << endl;
    } else {
        free();
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0xFF, 0xFF));
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if(texture == NULL) {
            cout << "No se ha podido crear la textura: " << SDL_GetError() << endl;
        } else {
            width = surface->w;
            height = surface->h;
        }
    }
    SDL_FreeSurface(surface);
    return texture != NULL;
}

void Texture::free() {
    if(texture != NULL) {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

void Texture::render(int x, int y, SDL_Rect *clip) {
    SDL_Rect rect = {x, y, width, height};
    if(clip != NULL) {
        rect.w = clip->w;
        rect.h = clip->h;
    }
    SDL_RenderCopy(renderer, texture, clip, &rect);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

const int WALKING_FRAMES = 4;
SDL_Rect clips[WALKING_FRAMES];
Texture texture;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "No se ha podido inicializar SDL: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "No se ha podido poner la calidad de escalado en lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Animaciones", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "No se ha podido crear la ventana: " << SDL_GetError() << endl;
            success = false;
        } else {
            //Al modo del renderer le ponemos tambien la sincronizacion vertical para que refresque con la pantalla:
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL) {
                cout << "El renderer no ha sido creado: " << SDL_GetError() << endl;
                success = false;
            } else {
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << "No ha podido ser inicializado el modulo de imagenes: " << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!texture.loadFromFile("assets/lesson14/foo.png")) {
        success = false;
    } else {
        //Creamos la posición de los distintos frames de la animacion:
        clips[0].x = 0;
        clips[0].y = 0;
        clips[0].w = 64;
        clips[0].h = 205;

        clips[1].x = 64;
        clips[1].y = 0;
        clips[1].w = 64;
        clips[1].h = 205;

        clips[2].x = 128;
        clips[2].y = 0;
        clips[2].w = 64;
        clips[2].h = 205;

        clips[3].x = 196;
        clips[3].y = 0;
        clips[3].w = 64;
        clips[3].h = 205;
    }

    return success;
}

void close() {
    texture.free();
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
            int frame = 0;
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }

                //La velocidad será de 10 frames por segundo (puesto que suponemos 60fps al poner Vsync):
                if(++frame/6 >= WALKING_FRAMES) {
                    frame = 0;
                }

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                //Pintamos el frame correspondiente:
                SDL_Rect *currentClip = &clips[frame/6];
                texture.render((SCREEN_WIDTH - currentClip->w)/2, (SCREEN_HEIGHT - currentClip->w)/2, currentClip);

                SDL_RenderPresent(renderer);
            }
        }
    }

    close();
    return 0;
}
