#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Renderer *renderer;
SDL_Window *window;

class Texture {
    public:
        ~Texture();

        bool createBlank(int w, int h, SDL_TextureAccess access = SDL_TEXTUREACCESS_STREAMING);
        bool loadFromFile(string path);
        void free();
        void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0, SDL_Point *center = NULL);

        int getWidth();
        int getHeight();

        //Seteamos la textura como render target:
        void setAsRenderTarget();

        //Manipuladores de pixeles:
        bool lockTexture();
        bool unlockTexture();
        void *getPixels();
        void copyPixels(void *pix);
        int getPitch();
        Uint32 getPixel32(unsigned int x, unsigned int y);
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
        SDL_Surface *formattedSurface = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA8888, 0);
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        if(formattedSurface == nullptr) {
            cout << SDL_GetError() << endl;
        } else {
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
            if(texture == nullptr) {
                cout << SDL_GetError() << endl;
            } else {
                //Habilitamos el blending:
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                //Bloqueamos la textura:
                SDL_LockTexture(texture, &formattedSurface->clip_rect, &pixels, &pitch);
                //Copiamos los pixeles:
                memcpy(pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

                width = formattedSurface->w;
                height = formattedSurface->h;

                Uint32 *pixelsInteger = (Uint32*)pixels;
                int pixelCount = (pitch/4)*height;
                //Map colors:
                Uint32 colorKey = SDL_MapRGB(formattedSurface->format, 0, 0xFF, 0xFF);
                Uint32 transparent = SDL_MapRGBA(formattedSurface->format, 0xFF, 0xFF, 0xFF, 0x00);

                //Los ponemos de forma manual:
                for(int i = 0; i < pixelCount; i++) {
                    if(pixelsInteger[i] == colorKey) {
                        pixelsInteger[i] = transparent;
                    }
                }

                //Desbloqueamos:
                SDL_UnlockTexture(texture);
                pixels = nullptr;
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

void Texture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center) {
    SDL_Rect rect = {x, y, width, height};
    if(clip != nullptr) {
        rect.w = clip->w;
        rect.h = clip->h;
    }
    SDL_RenderCopyEx(renderer, texture, clip, &rect, angle, center, SDL_FLIP_NONE);
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

Uint32 Texture::getPixel32(unsigned int x, unsigned int y) {
    //Convertimos los pixeles a 32 bits:
    Uint32* pixelsInteger = (Uint32*) pixels;

    //Getter del pixel requerido:
    return pixelsInteger[y * (pitch/4) + x];
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

bool Texture::createBlank(int w, int h, SDL_TextureAccess access) {
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, access, w, h);
    if(texture == nullptr) {
        cout << "No se ha podido crear la textura en blanco: " << SDL_GetError() << endl;
    } else {
        width = w;
        height = h;
    }
    return texture != nullptr;
}

void Texture::copyPixels(void *pix) {
    if(pixels != nullptr) {
        memcpy(pixels, pix, pitch*height);
    }
}

void Texture::setAsRenderTarget() {
    SDL_SetRenderTarget(renderer, texture);
}

Texture targetTexture;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Streaming textures", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    if(!targetTexture.createBlank(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_TEXTUREACCESS_TARGET)) {
        success = false;
    }

    return success;
}

void close() {
    targetTexture.free();

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

            //Angulo de la forma que vamos a dibujar y centro de la pantalla:
            double angle = 0;
            SDL_Point screenCenter = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};

            //Pintamos la pantalla de blanco por primera vez:
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);

            while(!quit) {
                while(SDL_PollEvent(&e)) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }

                angle += 2;
                if(angle > 360) {
                    angle -= 360;
                }

                //Seteamos la textura como objeto del renderizado:
                targetTexture.setAsRenderTarget();

                //Limpiamos screen:
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                //Renderizamos un rectangulo rojo relleno:
                SDL_Rect fillRect = {SCREEN_WIDTH/4, SCREEN_HEIGHT/4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
                SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
                SDL_RenderFillRect(renderer, &fillRect);

                //Renderizamos un rectangulo verde:
                SDL_Rect outlineRect = {SCREEN_WIDTH/6, SCREEN_HEIGHT/6, SCREEN_WIDTH*2/3, SCREEN_HEIGHT*2/3};
                SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
                SDL_RenderDrawRect(renderer, &outlineRect);

                //Renderizamos una linea horizontal azul:
                SDL_SetRenderDrawColor(renderer, 0, 0, 0xFF, 0xFF);
                SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2);

                //Dibujamos una linea vertical amarilla de puntos:
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0, 0xFF);
                for(int i = 0; i < SCREEN_HEIGHT; i += 4) {
                    SDL_RenderDrawPoint(renderer, SCREEN_WIDTH/2, i);
                }

                //Reset del target:
                SDL_SetRenderTarget(renderer, nullptr);

                //Y renderizamos finalmente la superficie:
                targetTexture.render(0, 0, nullptr, angle, &screenCenter);

                //Presentamos:
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}

