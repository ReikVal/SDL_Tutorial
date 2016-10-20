#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Renderer *renderer;
SDL_Window *window;

class Texture {
    public:
        ~Texture();

        bool loadFromFile(string path);
        void free();
        void render(int x, int y, SDL_Rect *clip = NULL);

        int getWidth();
        int getHeight();

        //Manipuladores de pixeles:
        bool lockTexture();
        bool unlockTexture();
        void *getPixels();
        int getPitch();
    private:
        SDL_Texture *texture;
        void *pixels;
        int pitch;

        int width{0};
        int height{0};
};

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(string path) {
    SDL_Surface *surf = IMG_Load(path.c_str());
    if(surf == nullptr) {
        cout << SDL_GetError() << endl;
    } else {
        free();
        SDL_Surface *formattedSurface = SDL_ConvertSurface(surf, SDL_GetWindowSurface(window)->format, 0);
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        if(formattedSurface == nullptr) {
            cout << SDL_GetError() << endl;
        } else {
            texture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
            if(texture == nullptr) {
                cout << SDL_GetError() << endl;
            } else {
                //Bloqueamos la textura:
                SDL_LockTexture(texture, nullptr, &pixels, &pitch);
                //Copiamos los pixeles:
                memcpy(pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);
                //Desbloqueamos:
                SDL_UnlockTexture(texture);
                pixels = nullptr;

                width = formattedSurface->w;
                height = formattedSurface->h;
            }
            SDL_FreeSurface(formattedSurface);
        }

        SDL_FreeSurface(surf);
    }

    return texture != nullptr;
}

void Texture::free() {
    if(texture != nullptr) {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

void Texture::render(int x, int y, SDL_Rect *clip) {
    SDL_Rect rect = {x, y, width, height};
    if(clip != nullptr) {
        rect.w = clip->w;
        rect.h = clip->h;
    }
    SDL_RenderCopy(renderer, texture, clip, &rect);
}

bool Texture::lockTexture() {
    bool success = true;

    if(pixels != nullptr) {
        cout << "Ya esta bloqueada" << endl;
        success = false;
    } else {
        if(SDL_LockTexture(texture, nullptr, &pixels, &pitch) != 0) {
            cout << "No se ha podido bloquear" << endl;
            success = false;
        }
    }

    return success;
}

bool Texture::unlockTexture() {
    bool success = true;

    if(pixels == nullptr) {
        cout << "Ya esta desbloqueada" << endl;
        success = false;
    } else {
        SDL_UnlockTexture(texture);
        pixels = nullptr;
        pitch = 0;
    }

    return success;
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

void *Texture::getPixels() {
    return pixels;
}

int Texture::getPitch() {
    return pitch;
}

Texture fooTexture;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Manipulacion de texturas", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == nullptr) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == nullptr) {
                cout << SDL_GetError() << endl;
                success = false;
            } else {
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!fooTexture.loadFromFile("assets/lesson40/foo.png")) {
        success = false;
    } else {
        //Bloqueamos para poder hacer cosas con los pixeles:
        fooTexture.lockTexture();
        //Get pixel data para hacer el color key de forma manual:
        Uint32 *pixels = (Uint32*)fooTexture.getPixels();
        int pixelCount = (fooTexture.getPitch()/4)*fooTexture.getHeight();
        //Map colors:
        Uint32 colorKey = SDL_MapRGB(SDL_GetWindowSurface(window)->format, 0, 0xFF, 0xFF);
        Uint32 transparent = SDL_MapRGBA(SDL_GetWindowSurface(window)->format, 0xFF, 0xFF, 0xFF, 0x00);

        //Los ponemos de forma manual:
        for(int i = 0; i < pixelCount; i++) {
            if(pixels[i] == colorKey) {
                pixels[i] = transparent;
            }
        }

        //Unlock:
        fooTexture.unlockTexture();
    }

    return success;
}

void close() {
    fooTexture.free();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
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
                //Limpiamos screen:
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                //Renderizamos la superficie:
                fooTexture.render((SCREEN_WIDTH - fooTexture.getWidth())/2, (SCREEN_HEIGHT - fooTexture.getHeight())/2);

                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
