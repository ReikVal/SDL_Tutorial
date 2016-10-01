#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;

class Texture {
    public:
        Texture();
        ~Texture();

        bool loadFromFile(string path);
        void free();
        void render(int x, int y, SDL_Rect *clip = NULL);

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
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL) {
        cout << "No superficie: " << IMG_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(texture == NULL) {
            cout << "No textura: " << SDL_GetError() << endl;
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
    SDL_Rect renderRect = {x, y, width, height};
    if(clip != NULL) {
        renderRect.w = clip->w;
        renderRect.h = clip->h;
    }
    SDL_RenderCopy(renderer, texture, clip, &renderRect);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

enum ButtonSprite {
    mouseOut = 0,
    mouseOver = 1,
    mouseDown = 2,
    mouseUp = 3,
    mouseTotal = 4
};

//Creamos 4 botones:
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

SDL_Rect spriteRects[mouseTotal];
Texture spriteSheet;

class Button {
    public:
        Button();
        //Setter de la posicion:
        void setPosition(int x, int y);
        //Handler de los eventos:
        void handleEvent(SDL_Event *e);
        //Renderizado:
        void render();
    private:
        //Posicion:
        SDL_Point position;
        //El sprite que apunta:
        ButtonSprite currentSprite;
};

Button::Button() {
    currentSprite = mouseOut;
    position.x = 0;
    position.y = 0;
}

void Button::setPosition(int x, int y) {
    position.x = x;
    position.y = y;
}

void Button::handleEvent(SDL_Event *e) {
    //Si un evento de raton sucede:
    if(e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
        //Obtenemos la posición y la seteamos:
        int x, y;
        SDL_GetMouseState(&x, &y);

        //Y comprobamos si estamos dentro del boton:
        bool outside = x < position.x || x > position.x + BUTTON_WIDTH || y < position.y || y > position.y + BUTTON_HEIGHT;

        if(outside) {
            currentSprite = mouseOut;
        } else {
            switch(e->type) {
                case SDL_MOUSEMOTION:
                    currentSprite = mouseOver;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    currentSprite = mouseDown;
                    break;
                case SDL_MOUSEBUTTONUP:
                    currentSprite = mouseUp;
                    break;
            }
        }
    }
}

void Button::render() {
    spriteSheet.render(position.x, position.y, &spriteRects[currentSprite]);
}

Button buttons[TOTAL_BUTTONS];

bool init() {
    bool success = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "No SDL: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
            cout << "No escalado lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Eventos de raton", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << "No ventana: " << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL) {
                cout << "No renderer: " << SDL_GetError() << endl;
                success = false;
            } else {
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << "No modulo de imagenes: " << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }
    return success;
}

bool loadMedia() {
    bool success = true;

    if(!spriteSheet.loadFromFile("assets/lesson17/button.png")) {
        success = false;
    } else {
        for(int i = 0; i < mouseTotal; i++) {
            spriteRects[i].x = 0;
            spriteRects[i].y = i*BUTTON_HEIGHT;
            spriteRects[i].w = BUTTON_WIDTH;
            spriteRects[i].h = BUTTON_HEIGHT;
        }
        buttons[0].setPosition(0, 0);
        buttons[1].setPosition(SCREEN_WIDTH-BUTTON_WIDTH, 0);
        buttons[2].setPosition(0, SCREEN_HEIGHT-BUTTON_HEIGHT);
        buttons[3].setPosition(SCREEN_WIDTH-BUTTON_WIDTH, SCREEN_HEIGHT-BUTTON_HEIGHT);
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
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                    for(int i = 0; i < TOTAL_BUTTONS; i++) {
                        buttons[i].handleEvent(&e);
                    }
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                for(int i = 0; i < TOTAL_BUTTONS; i++) {
                    buttons[i].render();
                }
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
