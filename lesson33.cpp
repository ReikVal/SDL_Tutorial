#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int TOTAL_DATA = 10;

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;
Sint32 data[TOTAL_DATA];

template <typename T>
string to_string(T n) {
    stringstream ss;
    ss << n;
    return ss.str();
}

class Texture {
    public:
        ~Texture();

        bool loadFromRendererText(string inputText, SDL_Color color);
        void free();
        void render(int x, int y);

        int getWidth();
        int getHeight();
    private:
        SDL_Texture *texture;
        int width{0};
        int height{0};
};

Texture::~Texture() {
    free();
}

bool Texture::loadFromRendererText(string inputText, SDL_Color color) {
    SDL_Surface *surf = TTF_RenderText_Solid(font, inputText.c_str(), color);
    if(surf == nullptr) {
        cout << TTF_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        if(texture == nullptr) {
            cout << SDL_GetError() << endl;
        } else {
            width = surf->w;
            height = surf->h;
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

void Texture::render(int x, int y) {
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture introduceTexto;
Texture texto[TOTAL_DATA];

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("IO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == nullptr) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == nullptr) {
                cout << SDL_GetError() << endl;
                success = false;
            } else {
                if(TTF_Init() == -1) {
                    cout << TTF_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    //Abrimos el archivo para leer:
    SDL_RWops *file = SDL_RWFromFile("assets/lesson33/nums.bin", "r+b");

    if(file == nullptr) {
        file = SDL_RWFromFile("assets/lesson33/nums.bin", "w+b");
        if(file != nullptr) {
            cout << "Archivo creado" << endl;
            for(int i = 0; i < TOTAL_DATA; i++) {
                data[i] = 0;
                SDL_RWwrite(file, &data[i], sizeof(Sint32), 1);
            }
            //Cerramos el archivo:
            SDL_RWclose(file);
        } else {
            cout << "No se ha podido crear el archivo: " << SDL_GetError() << endl;
            success = false;
        }
    } else {
        cout << "Leyendo archivo: " << endl;
        //Leemos y lo introducimos en data
        for(int i = 0; i < TOTAL_DATA; i++) {
            SDL_RWread(file, &data[i], sizeof(Sint32), 1);
        }
        //Cerramos:
        SDL_RWclose(file);
    }

    font = TTF_OpenFont("assets/lesson33/lazy.ttf", 18);

    if(font == nullptr) {
        cout << TTF_GetError() << endl;
        success = false;
    } else {
        SDL_Color color = {0, 0, 0, 0xFF};
        if(!introduceTexto.loadFromRendererText("Datos: ", color)) {
            success = false;
        }
    }

    SDL_Color color = {0, 0, 0, 0xFF};
    SDL_Color highlight = {0xFF, 0, 0, 0xFF};

    texto[0].loadFromRendererText(to_string(data[0]), highlight);
    for(int i = 1; i < TOTAL_DATA; i++) {
        texto[i].loadFromRendererText(to_string(data[i]), color);
    }

    return success;
}

void close() {
    //Guardamos datos:
    SDL_RWops *file = SDL_RWFromFile("assets/lesson33/nums.bin", "w+b");
    if(file != nullptr) {
        for(int i = 0; i < TOTAL_DATA; i++) {
            SDL_RWwrite(file, &data[i], sizeof(Sint32), 1);
        }
        SDL_RWclose(file);
    }
    for(int i = 0; i < TOTAL_DATA; i++) {
        texto[i].free();
    }
    introduceTexto.free();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;
            SDL_Event e;
            SDL_Color color = {0, 0, 0, 0xFF};
            SDL_Color highlight = {0xFF, 0, 0, 0xFF};
            int currentData = 0;

            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_KEYDOWN) {
                        switch(e.key.keysym.sym) {
                            case SDLK_UP:
                                texto[currentData].loadFromRendererText(to_string(data[currentData]), color);
                                if(--currentData < 0) {
                                    currentData = TOTAL_DATA - 1;
                                }
                                texto[currentData].loadFromRendererText(to_string(data[currentData]), highlight);
                                break;
                            case SDLK_DOWN:
                                texto[currentData].loadFromRendererText(to_string(data[currentData]), color);
                                if(++currentData == TOTAL_DATA) {
                                    currentData = 0;
                                }
                                texto[currentData].loadFromRendererText(to_string(data[currentData]), highlight);
                                break;
                            case SDLK_LEFT:
                                --data[currentData];
                                texto[currentData].loadFromRendererText(to_string(data[currentData]), highlight);
                                break;
                            case SDLK_RIGHT:
                                ++data[currentData];
                                texto[currentData].loadFromRendererText(to_string(data[currentData]), highlight);
                                break;
                        }
                    }
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                introduceTexto.render((SCREEN_WIDTH - introduceTexto.getWidth())/2, 0);
                for(int i = 0; i < TOTAL_DATA; i++) {
                    texto[i].render((SCREEN_WIDTH - introduceTexto.getWidth())/2, introduceTexto.getHeight() + i*texto[i].getHeight());
                }
                SDL_RenderPresent(renderer);
            }
            SDL_StopTextInput();
        }
    }
    close();
    return 0;
}
