#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
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
        void render(int x, int y);
    private:
        SDL_Texture *texture;
};

Texture::Texture() {
    texture = NULL;
}

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(string path) {
    SDL_Surface *surface = IMG_Load(path.c_str());
    if(surface == NULL) {
        cout << IMG_GetError() << endl;
    } else {
        free();
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if(surface == NULL) {
            cout << SDL_GetError() << endl;
        }
        SDL_FreeSurface(surface);
    }
    return texture != NULL;
}

void Texture::free() {
    if(texture != NULL) {
        SDL_DestroyTexture(texture);
    }
}

void Texture::render(int x, int y) {
    SDL_Rect rect = {x, y, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

Texture imagen;

//Creamos los punteros a la musica y efectos que usaremos:
Mix_Music *musica = NULL;
Mix_Chunk *efecto1 = NULL;
Mix_Chunk *efecto2 = NULL;
Mix_Chunk *efecto3 = NULL;
Mix_Chunk *efecto4 = NULL;

bool init() {
    bool success = true;

    //Abrimos SDL tambien con el audio:
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        w = SDL_CreateWindow("Jugando con el audio", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(w == NULL) {
            cout << SDL_GetError() << endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL) {
                cout << SDL_GetError() << endl;
                success = false;
            } else {
                int imgFlags = IMG_INIT_PNG;
                if((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
                    cout << IMG_GetError() << endl;
                    success = false;
                } else if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
                    //Con esto hemos inicializado el audio con el formato por defecto, 44.1KHz, 2 canales y 2048 de tamaño de chunk (like a buffer)
                    cout << "No ha sido inicializado el audio: " << Mix_GetError() << endl;
                    success = false;
                    //Hay que destacar, que existe Mix_Init, que se usa como IMG_Init (con las flags) que sirve para dar soporte a MOD, MP3, OGG, FLAC
                    //pero como usaremos .wav, no hará falta:
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    if(!imagen.loadFromFile("assets/lesson21/prompt.png")) {
        success = false;
    }

    musica = Mix_LoadMUS("assets/lesson21/beat.wav");
    if(musica == NULL) {
        cout << "No se ha podido cargar el wav: " << Mix_GetError() << endl;
        success = false;
    }

    efecto1 = Mix_LoadWAV("assets/lesson21/scratch.wav");
    if(efecto1 == NULL) {
        cout << "No se ha podido cargar el wav: " << Mix_GetError() << endl;
        success = false;
    }

    efecto2 = Mix_LoadWAV("assets/lesson21/high.wav");
    if(efecto2 == NULL) {
        cout << "No se ha podido cargar el wav: " << Mix_GetError() << endl;
        success = false;
    }

    efecto3 = Mix_LoadWAV("assets/lesson21/medium.wav");
    if(efecto3 == NULL) {
        cout << "No se ha podido cargar el wav: " << Mix_GetError() << endl;
        success = false;
    }

    efecto4 = Mix_LoadWAV("assets/lesson21/low.wav");
    if(efecto4 == NULL) {
        cout << "No se ha podido cargar el wav: " << Mix_GetError() << endl;
        success = false;
    }

    return success;
}

void close() {
    imagen.free();
    //Liberamos recursos de la musica y los efectos:
    Mix_FreeMusic(musica);
    Mix_FreeChunk(efecto1);
    Mix_FreeChunk(efecto2);
    Mix_FreeChunk(efecto3);
    Mix_FreeChunk(efecto4);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(w);

    //Nota, si hubiesemos inicializado un flag, para asegurarnos de quitarlo deberiamos de hacer: "while(Mix_Init(0)) Mix_Quit();"
    Mix_Quit();
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
                    } else if(e.type == SDL_KEYDOWN) {
                        switch(e.key.keysym.sym) {
                            case SDLK_1:
                                Mix_PlayChannel(-1, efecto1, 0);
                                break;
                            case SDLK_2:
                                Mix_PlayChannel(-1, efecto2, 0);
                                break;
                            case SDLK_3:
                                Mix_PlayChannel(-1, efecto3, 0);
                                break;
                            case SDLK_4:
                                Mix_PlayChannel(-1, efecto4, 0);
                                break;
                            case SDLK_9:
                                if(Mix_PlayingMusic() == 0) {
                                    Mix_PlayMusic(musica, -1);
                                } else if(Mix_PausedMusic() == 1) {
                                    Mix_ResumeMusic();
                                } else {
                                    Mix_PauseMusic();
                                }
                                break;
                            case SDLK_0:
                                Mix_HaltMusic();
                                break;
                        }
                    }
                }
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
                imagen.render(0, 0);
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
