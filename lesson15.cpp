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

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;

class Texture {
    public:
        Texture();
        ~Texture();

        bool loadFromFile(string path);

        void free();

        //Añadimos angulo de rotacion, centro y volteo si fuese necesario.
        void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

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
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL) {
        cout << "No se ha podido cargar la superficie: " << IMG_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(texture == NULL) {
            cout << "No se ha podido crear la textura: " << SDL_GetError() << endl;
        } else {
            width = loadedSurface->w;
            height = loadedSurface->h;
        }
    }
    SDL_FreeSurface(loadedSurface);
    return texture != NULL;
}

void Texture::free() {
    if(texture != NULL) {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

void Texture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip) {
    SDL_Rect renderRect = {x, y, width, height};
    if(clip != NULL) {
        renderRect.w = clip->w;
        renderRect.h = clip->h;
    }
    SDL_RenderCopyEx(renderer, texture, clip, &renderRect, angle, center, flip);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture texture;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha podido ser inicializado: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "No hay escalado lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Rotando y volteando la textura.", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "La ventana no ha sido creada: " << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL) {
                cout << "El renderer no ha sido creado: " << SDL_GetError() << endl;
                success = false;
            } else {
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << "El modulo de imagenes no ha podido ser iniciado: " << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!texture.loadFromFile("assets/lesson15/arrow.png")) {
        success = false;
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
            //Angulo:
            double angle = 0;
            //Flip:
            SDL_RendererFlip flip = SDL_FLIP_NONE;

            while(!quit) {
                while(SDL_PollEvent(&e)) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_KEYDOWN) {
                        switch(e.key.keysym.sym) {
                            case SDLK_q:
                                flip = SDL_FLIP_HORIZONTAL;
                                break;
                            case SDLK_w:
                                flip = SDL_FLIP_NONE;
                                break;
                            case SDLK_e:
                                flip = SDL_FLIP_VERTICAL;
                                break;
                            case SDLK_s:
                                //Es necesario el casting para añadir los dos flag en algunos compiladores:
                                flip = (SDL_RendererFlip) (SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL);
                                break;
                            case SDLK_a:
                                angle -= 32;
                                break;
                            case SDLK_d:
                                angle += 32;
                                break;
                        }
                    }
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                texture.render((SCREEN_WIDTH - texture.getWidth())/2, (SCREEN_HEIGHT - texture.getHeight())/2, NULL, angle, NULL, flip);

                SDL_RenderPresent(renderer);
            }
        }
    }

    close();
    return 0;
}
