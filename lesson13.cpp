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

        void setBlendMode(SDL_BlendMode blendMode);

        void setAlpha(Uint8 alpha);

        void render(int x, int y);

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
        cout << "No ha podido ser cargada la imagen: " << IMG_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(texture == NULL) {
            cout << "No ha podido ser creada la textura: " << SDL_GetError() << endl;
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

void Texture::setBlendMode(SDL_BlendMode blendMode) {
    //Ponemos el blendMode seleccionado:
    SDL_SetTextureBlendMode(texture, blendMode);
}

void Texture::setAlpha(Uint8 alpha) {
    //Ponemos a la textura el alpha que queramos, de 0 a 255:
    SDL_SetTextureAlphaMod(texture, alpha);
}

void Texture::render(int x, int y) {
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture white;
Texture black;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "No ha podido ser inicializado SDL: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "El escalado no ha podido ser puesto en calidad lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Trabajando con alpha", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "No se ha podido crear la ventana: " << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
            if(renderer == NULL) {
                cout << "No se ha podido crear el renderer: " << SDL_GetError() << endl;
                success = false;
            } else {
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << "No se ha podido inicializar el soporte para PNG: " << IMG_GetError() << endl;
                    success = false;
                }

            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!white.loadFromFile("assets/lesson13/fadeout.png")) {
        success = false;
    } else if(!black.loadFromFile("assets/lesson13/fadein.png")) {
        success = false;
    } else {
        //Le decimos que el blend se puede hacer con el alpha:
        white.setBlendMode(SDL_BLENDMODE_BLEND);
    }

    return success;
}

void close() {
    white.free();
    black.free();
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
            Uint8 alpha = 255;
            while(!quit) {
                while(SDL_PollEvent(&e)) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_KEYDOWN) {
                        switch(e.key.keysym.sym) {
                            case SDLK_w:
                                if(alpha + 32 > 255) {
                                    alpha = 255;
                                } else {
                                    alpha += 32;
                                }
                                break;
                            case SDLK_s:
                                if(alpha - 32 < 0) {
                                    alpha = 0;
                                } else {
                                    alpha -= 32;
                                }
                                break;
                        }
                    }
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                //Añadimos el alpha a la textura y pintamos. Es importante pintar el negro antes, ya que es el background:
                white.setAlpha(alpha);
                black.render(0, 0);
                white.render(0, 0);
                //Presentamos pantalla:
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
