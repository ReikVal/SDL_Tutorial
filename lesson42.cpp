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

        bool createBlank(int w, int h);
        bool loadFromFile(string path);
        void free();
        void render(int x, int y, SDL_Rect *clip = NULL);

        int getWidth();
        int getHeight();

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

bool Texture::createBlank(int w, int h) {
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
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

Texture streamingTexture;

//Animation stream:
class DataStream {
    public:
        //Carga los datos inciales:
        bool loadMedia();
        //Liberar recursos:
        void free();
        //Getter del buffer:
        void* getBuffer();
    private:
        SDL_Surface *images[4];
        int currentImage{0};
        int delayFrames{4};
};

bool DataStream::loadMedia() {
    bool success = true;

    for(int i = 0; i < 4; i++) {
        std::stringstream ss;
        ss << "assets/lesson42/foo_walk_" << i << ".png";
        SDL_Surface *surf = IMG_Load(ss.str().c_str());
        if(surf == nullptr) {
            cout << "No se ha podido cargar la imagen: " << IMG_GetError() << endl;
            success = false;
        } else {
            images[i] = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA8888, 0);
        }

        SDL_FreeSurface(surf);
    }

    return success;
}

void DataStream::free() {
    for(int i = 0; i < 4; i++) {
        SDL_FreeSurface(images[i]);
    }
}

void *DataStream::getBuffer() {
    delayFrames--;
    if(delayFrames == 0) {
        currentImage++;
        delayFrames = 4;
    }

    if(currentImage == 4) {
        currentImage = 0;
    }

    return images[currentImage]->pixels;
}

DataStream dataStream;

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
    if(!streamingTexture.createBlank(64, 205)) {
        success = false;
    }

    if(!dataStream.loadMedia()) {
        success = false;
    }

    return success;
}

void close() {
    streamingTexture.free();
    dataStream.free();

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

                //Copiamos la imagen del buffer:
                streamingTexture.lockTexture();
                streamingTexture.copyPixels(dataStream.getBuffer());
                streamingTexture.unlockTexture();

                //Renderizamos:
                streamingTexture.render((SCREEN_WIDTH - streamingTexture.getWidth())/2, (SCREEN_HEIGHT - streamingTexture.getHeight())/2);

                //Presentamos:
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
