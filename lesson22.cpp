#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

class Texture {
    public:
        Texture();
        ~Texture();
        bool loadFromFile(string path);
        bool loadFromRenderedText(string textureText, SDL_Color textColor);
        void free();
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

//Esta función no hace falta para este tutorial:
bool Texture::loadFromFile(string path) {
    SDL_Surface *surf = IMG_Load(path.c_str());
    if(surf == NULL) {
        cout << "No superficie: " << IMG_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        if(texture == NULL) {
            cout << "No textura: " << SDL_GetError() << endl;
        } else {
            width = surf->w;
            height = surf->h;
        }
        SDL_FreeSurface(surf);
    }
    return texture != NULL;
}

bool Texture::loadFromRenderedText(string textureText, SDL_Color textColor) {
    SDL_Surface *surf = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
    if(surf == NULL) {
        cout << "No superficie: " << TTF_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        if(texture == NULL) {
            cout << "No textura: " << SDL_GetError() << endl;
        } else {
            width = surf->w;
            height = surf->h;
        }
        SDL_FreeSurface(surf);
    }
    return texture != NULL;
}

void Texture::free() {
    if(texture != NULL) {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

void Texture::render(int x, int y) {
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture resetText;
Texture timeText;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Trabajando con timers", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL) {
                cout << SDL_GetError() << endl;
                success = false;
            } else {
                if(TTF_Init() == -1) {
                    cout << "No se ha podido abrir el modulo de texto TTF: " << TTF_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    font = TTF_OpenFont("assets/lesson22/lazy.ttf", 18);
    if(font == NULL) {
        cout << "No se ha podido cargar la fuente: " << TTF_GetError() << endl;
        success = false;
    } else {
        //Creamos el texto de arriba:
        SDL_Color textColor = {0, 0, 0, 255};
        if(!resetText.loadFromRenderedText("Pulsa Intro para reiniciar el tiempo", textColor)) {
            success = false;
        }
    }

    return success;
}

void close() {
    resetText.free();
    timeText.free();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if(init()) {
        if(loadMedia()) {
            bool quit = false;
            SDL_Event e;
            SDL_Color textColor = {0, 0, 0, 255};
            Uint32 startTime = 0;
            //Creamos un string stream para ir cambiando el texto:
            stringstream timeString;
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                        startTime = SDL_GetTicks();
                    }
                }

                timeString.str("");
                timeString << "Milisegundos pasados: " << SDL_GetTicks() - startTime;
                if(!timeText.loadFromRenderedText(timeString.str().c_str(), textColor)) {
                    quit = true;
                } else {
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderClear(renderer);
                    resetText.render((SCREEN_WIDTH - resetText.getWidth())/2, 0);
                    timeText.render(200, (SCREEN_HEIGHT - timeText.getHeight())/2);
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }
    close();
    return 0;
}
