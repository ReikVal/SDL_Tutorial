#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;

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
Texture texto;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        window = SDL_CreateWindow("Introduccion de texto", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    font = TTF_OpenFont("assets/lesson32/lazy.ttf", 18);

    if(font == nullptr) {
        cout << TTF_GetError() << endl;
        success = false;
    } else {
        SDL_Color color = {0, 0, 0, 0xFF};
        if(!introduceTexto.loadFromRendererText("Introduce un texto: ", color)) {
            success = false;
        }
    }

    return success;
}

void close() {
    introduceTexto.free();
    texto.free();
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
            string inputText = "Texto";
            texto.loadFromRendererText(inputText, color);

            //Habilitamos la introduccion de texto:
            SDL_StartTextInput();
            while(!quit) {
                bool renderText = false;
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    } else if(e.type == SDL_KEYDOWN) {
                        //Vamos a comprobar si se borran letras y si se copia o se pega:
                        if(e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0) {
                            inputText.pop_back();
                            renderText = true;
                        } else if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                            SDL_SetClipboardText(inputText.c_str());
                        } else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                            inputText = SDL_GetClipboardText();
                            renderText = true;
                        }
                    } else if(e.type == SDL_TEXTINPUT) {
                        if(!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && (SDL_GetModState() & KMOD_CTRL))) {
                            inputText += e.text.text;
                            renderText = true;
                        }
                    }
                }
                if(renderText) {
                    if(inputText != "") {
                        texto.loadFromRendererText(inputText, color);
                    } else {
                        texto.loadFromRendererText(" ", color);
                    }
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                introduceTexto.render((SCREEN_WIDTH - introduceTexto.getWidth())/2, 0);
                texto.render((SCREEN_WIDTH - introduceTexto.getWidth())/2, introduceTexto.getHeight());
                SDL_RenderPresent(renderer);
            }
            SDL_StopTextInput();
        }
    }
    close();
    return 0;
}
