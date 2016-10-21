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

Texture bitmapTexture;

class BitmapFont {
    public:
        //Genera la fuente:
        bool buildFont(Texture *bitmap);

        //Renderiza el texto:
        void renderText(int x, int y, std::string text);
    private:
        //La textura de la fuente:
        Texture *bitmap;
        //Los caracteres en la supercicie:
        SDL_Rect chars[256];
        //Espaciado:
        int newLine{0};
        int space{0};

};

bool BitmapFont::buildFont(Texture *bitmap) {
    bool success = true;

    if(!bitmap->lockTexture()) {
        cout << "No se ha podido bloquear la textura" << endl;
        success = false;
    } else {
        //Background color:
        Uint32 bgColor = bitmap->getPixel32(0, 0);

        //Cell dimension:
        int cellW = bitmap->getWidth()/16;
        int cellH = bitmap->getHeight()/16;

        int top = cellH;
        int baseA = cellH;

        int currentChar = 0;

        for(int rows = 0; rows < 16; rows++) {
            for(int cols = 0; cols < 16; cols++) {
                //Offset:
                chars[currentChar].x = cellW * cols;
                chars[currentChar].y = cellH * rows;

                //Dimensiones:
                chars[currentChar].w = cellW;
                chars[currentChar].h = cellH;

                //Buscamos el lado izquierdo:
                for(int pCol = 0; pCol < cellW; pCol++) {
                    for(int pRow = 0; pRow < cellH; pRow++) {
                        int pX = (cellW * cols) + pCol;
                        int pY = (cellH * rows) + pRow;

                        if(bitmap->getPixel32(pX, pY) != bgColor) {
                            chars[currentChar].x = pX;

                            pCol = cellW;
                            pRow = cellH;
                        }
                    }
                }

                //Buscamos el lado derecho:
                for(int pColW = cellW - 1; pColW >= 0; pColW--) {
                    for(int pRowW = 0; pRowW < cellH; pRowW++) {
                        int pX = (cellW * cols) + pColW;
                        int pY = (cellH * rows) + pRowW;

                        if(bitmap->getPixel32(pX, pY) != bgColor) {
                            chars[currentChar].w = pX - chars[currentChar].x + 1;

                            pColW = -1;
                            pRowW = cellH;
                        }
                    }
                }

                //Buscamos el top:
                for(int pRow = 0; pRow < cellH; pRow++) {
                    for(int pCol = 0; pCol < cellW; pCol++) {
                        int pX = (cellW * cols) + pCol;
                        int pY = (cellH * rows) + pRow;

                        if(bitmap->getPixel32(pX, pY) != bgColor) {
                            if(pRow < top) {
                                top = pRow;
                            }

                            pCol = cellW;
                            pRow = cellH;
                        }
                    }
                }

                //Buscamos la parte de abajo de A:
                if(currentChar == 'A') {
                    for(int pRow = cellH - 1; pRow >= 0; pRow--) {
                        for(int pCol = 0; pCol < cellW; pCol++) {
                            int pX = (cellW * cols) + pCol;
                            int pY = (cellH * rows) + pRow;

                            if(bitmap->getPixel32(pX, pY) != bgColor) {
                                baseA = pRow;

                                pCol = cellW;
                                pRow = -1;
                            }
                        }
                    }
                }

                currentChar++;
            }
        }
        //Calculamos espacio:
        space = cellW/2;
        //Y nueva linea:
        newLine = baseA - top;

        //Loop por el exceso de pixeles en top:
        for(int i = 0; i < 256; i++) {
            chars[i].y += top;
            chars[i].h -= top;
        }

        bitmap->unlockTexture();
        this->bitmap = bitmap;
    }

    return success;
}

void BitmapFont::renderText(int x, int y, std::string text) {
    if(bitmap != nullptr) {
        int curX = x;
        int curY = y;
        for(unsigned int i = 0; i < text.length(); i++) {
            if(text[i] == ' ') {
                curX += space;
            } else if(text[i] == '\n') {
                curY += newLine;
                curX = x;
            } else {
                int ascii = (unsigned char)text[i];

                bitmap->render(curX, curY, &chars[ascii]);

                curX += chars[ascii].w +1;
            }
        }
    }
}

BitmapFont bitmapFont;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Bitmap font hechas a mano", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    if(!bitmapTexture.loadFromFile("assets/lesson41/lazyfont.png")) {
        success = false;
    } else {
        bitmapFont.buildFont(&bitmapTexture);
    }

    return success;
}

void close() {
    bitmapTexture.free();

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
                bitmapFont.renderText(0, 0, "Bitmap Font:\nABDCEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789");

                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}

