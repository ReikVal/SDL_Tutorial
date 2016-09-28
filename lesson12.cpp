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

        //Modulado de color:
        void setColor(Uint8 red, Uint8 green, Uint8 blue);

        //Render pasandole un rectangulo para hacer el clip:
        void render(int x, int y, SDL_Rect *clip = NULL);

        //Getters:
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
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL) {
        cout << "No ha podido ser cargada la superficie: " << IMG_GetError() << endl;
    } else {
        free();
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
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

void Texture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
    //Modularizamos la textura:
    SDL_SetTextureColorMod(texture, red, green, blue);
}

void Texture::render(int x, int y, SDL_Rect *clip) {
    SDL_Rect dest = {x, y, width, height};
    if(clip != NULL) {
        dest.w = clip->w;
        dest.h = clip->h;
    }
    SDL_RenderCopy(renderer, texture, clip, &dest);
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
            cout << "La calidad del escalado no ha podido ponerse en lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Trabajando con Modulacion de color", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                    cout << "Se ha producido un error al iniciar SDL_image: " << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!texture.loadFromFile("assets/lesson12/colors.png")) {
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
            Uint8 r = 255;
            Uint8 g = 255;
            Uint8 b = 255;
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_KEYDOWN) {
                        switch(e.key.keysym.sym) {
                            case SDLK_q:
                                r += 32;
                                break;
                            case SDLK_w:
                                g += 32;
                                break;
                            case SDLK_e:
                                b += 32;
                                break;
                            case SDLK_a:
                                r -= 32;
                                break;
                            case SDLK_s:
                                g -= 32;
                                break;
                            case SDLK_d:
                                b -= 32;
                                break;
                        }
                    }
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                //Modulamos el color:
                texture.setColor(r, g, b);
                texture.render(0, 0);
                //Y presentamos:
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
