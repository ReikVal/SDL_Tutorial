#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 540;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

//Es cierto que en el tutorial, se va ampliando esta clase conforme se va haciendo. Prefiero tener lo básico para centrarme en el concepto ahora mismo:
class Texture {
    public:
        Texture();
        ~Texture();
        //Cargaremos con esta funcion el TTF:
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

//Se carga igual que una imagen:
bool Texture::loadFromRenderedText(string textureText, SDL_Color textColor) {
    SDL_Surface *loadedSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
    if(loadedSurface == NULL) {
        cout << "No ha podido ser creada la fuente: " << TTF_GetError() << endl;
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

void Texture::render(int x, int y) {
    SDL_Rect dest = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

Texture text;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "No se ha podido inicializar SDL: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
            cout << "No hay escalado lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Textos", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "No se ha podido crear la ventana: " << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL) {
                cout << "No renderer: " << SDL_GetError() << endl;
                success = false;
            } else {
                //Por último iniciamos el modulo de texto ttf:
                if(TTF_Init() == -1) {
                    cout << "No se ha podido inicializar el modulo de texto: " << TTF_GetError() << endl;
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    //Aqui abrimos la fuente ttf y se la añadimos al objeto TTF_Font declarado globalmente:
    font = TTF_OpenFont("assets/lesson16/lazy.ttf", 20);
    if(font == NULL) {
        cout << "No se ha podido abrir la fuente: " << TTF_GetError() << endl;
        success = false;
    } else {
        SDL_Color textColor = {0, 0, 0};
        if(!text.loadFromRenderedText("El veloz murciélago hindú comía feliz cardillo y kiwi.", textColor)) {
            success = false;
        }
    }

    return success;
}

void close() {
    text.free();
    //Importante cerrar la fuente que se ha usado:
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);
    //Y cerrar el modulo:
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
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
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                //Pintamos el texto:
                text.render((SCREEN_WIDTH - text.getWidth())/2, (SCREEN_HEIGHT - text.getHeight())/2);

                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
