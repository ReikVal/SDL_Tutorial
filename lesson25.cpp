#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000/SCREEN_FPS;

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

class Texture {
    public:
        Texture();
        ~Texture();
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

Texture fpsText;

//Creamos una clase para hacer un Timer:
class Timer {
    public:
        //Constructor
        Timer();

        //Acciones:
        void start();
        void stop();
        void pause();
        void resume();

        //Getter del timer:
        Uint32 getTicks();

        //Getters:
        bool isStarted();
        bool isPaused();
    private:
        //Cuando el timer fue iniciado:
        Uint32 startTicks;
        //Cuando fue pausado:
        Uint32 pausedTicks;
        //Estados:
        bool paused;
        bool started;
};

Timer::Timer() {
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

//Metodos de accion para el timer:
void Timer::start() {
    startTicks = SDL_GetTicks();
    started = true;
}

void Timer::stop() {
    startTicks = 0;
    pausedTicks = 0;
    started = false;
    paused = false;
}

void Timer::pause() {
    if(started && !paused) {
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
}

void Timer::resume() {
    if(started && paused) {
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

//Getter del time real pasado:
Uint32 Timer::getTicks() {
    Uint32 result = 0;

    if(started) {
        result = paused?pausedTicks:SDL_GetTicks() - startTicks;
    }

    return result;
}

bool Timer::isStarted() {
    return started;
}

bool Timer::isPaused() {
    return paused;
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Timers: Cuarta parte, limitando manualmente los FPS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            //Como vamos a limitar los FPS, le quitamos el SDL_RENDERER_PRESENTVSYNC
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
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

    font = TTF_OpenFont("assets/lesson25/lazy.ttf", 18);
    if(font == NULL) {
        cout << "No se ha podido cargar la fuente: " << TTF_GetError() << endl;
        success = false;
    }

    return success;
}

void close() {
    fpsText.free();
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
            stringstream ss;
            SDL_Color color = {0, 0, 0, 255};
            int countedFrames = 0;
            //Timer para contar los FPS:
            Timer timer;
            timer.start();
            //Timer para capar los FPS:
            Timer capTimer;
            while(!quit) {
                capTimer.start();
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }

                //Añadimos los FPS al sstream:
                ss.str("");
                ss << "Average FPS: " << countedFrames/(timer.getTicks()/1000.f);
                fpsText.loadFromRenderedText(ss.str().c_str(), color);

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                fpsText.render(200, (SCREEN_HEIGHT - fpsText.getHeight())/2);
                SDL_RenderPresent(renderer);
                countedFrames++;
                //Vemos cuanto tiempo ha pasado:
                int frameTicks = capTimer.getTicks();
                if(frameTicks < SCREEN_TICKS_PER_FRAME) {
                    capTimer.stop();
                    //Son los milisegundos que deben pasar en un frame menos los que ya pasaron:
                    SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
                }
            }
        }
    }
    close();
    return 0;
}
