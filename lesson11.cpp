#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Renderer *renderer = NULL;
SDL_Window *w = NULL;

bool init();
bool loadMedia();
void close();

class Texture {
    public:
        Texture();
        ~Texture();

        bool loadFromFile(string path);

        void free();

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

Texture spriteSheet;
SDL_Rect spriteClips[4];

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha podido ser inicializado: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "La calidad del escalado no ha podido ponerse en lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Trabajando con SpriteSheets", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    if(!spriteSheet.loadFromFile("assets/lesson11/dots.png")) {
        success = false;
    } else {
        spriteClips[0].x = 0;
        spriteClips[0].y = 0;
        spriteClips[0].w = 100;
        spriteClips[0].h = 100;

        spriteClips[1].x = 100;
        spriteClips[1].y = 0;
        spriteClips[1].w = 100;
        spriteClips[1].h = 100;

        spriteClips[2].x = 0;
        spriteClips[2].y = 100;
        spriteClips[2].w = 100;
        spriteClips[2].h = 100;

        spriteClips[3].x = 100;
        spriteClips[3].y = 100;
        spriteClips[3].w = 100;
        spriteClips[3].h = 100;
    }

    return success;
}

void close() {
    spriteSheet.free();
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
            while(!quit) {
                while(SDL_PollEvent(&e)) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }
                //Limpiamos pantalla:
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                //Pintamos:
                spriteSheet.render(0, 0, &spriteClips[0]);
                spriteSheet.render(SCREEN_WIDTH - spriteClips[1].w, 0, &spriteClips[1]);
                spriteSheet.render(0, SCREEN_HEIGHT - spriteClips[2].h, &spriteClips[2]);
                spriteSheet.render(SCREEN_WIDTH - spriteClips[3].w, SCREEN_HEIGHT - spriteClips[3].h, &spriteClips[3]);

                //Presentamos:
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
