#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Creamos un wrapper para las texturas:
class Texture {
    public:
        Texture();
        ~Texture();

        //Para cargar imagen desde una ubicacion:
        bool loadFromFile(string);

        //Liberar textura:
        void free();

        //Renderizado:
        void render(int, int);

        //Getters para la dimension:
        int getWidth();
        int getHeight();
    private:
        //La textura en si:
        SDL_Texture *texture;

        //Dimensiones de la imagen:
        int width;
        int height;
};

SDL_Window *w = NULL;
SDL_Renderer *renderer = NULL;
Texture fooTexture;
Texture bgTexture;

bool init();
bool loadMedia();
void close();

//Constructor y destructor:
Texture::Texture() {
    texture = NULL;
    width = 0;
    height = 0;
}

Texture::~Texture() {
    free();
}

//Metodo para cargar la textura desde un archivo:
bool Texture::loadFromFile(string path) {
    free();
    //Textura final:
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL) {
        cout << "No ha sido cargada la superficie " << path << ": " << IMG_GetError() << endl;
    } else {
        //Aqui realizamos la novedad de esta leccion, que es que el color (0, 255, 255) sea transparente en la superficie que hemos cargado:
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        //Y seguimos como siempre, añadiendo también los miembros de la clase:
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(newTexture == NULL) {
            cout << "No ha podido ser creada la textura " << path << ": " << SDL_GetError() << endl;
        } else {
            width = loadedSurface->w;
            height = loadedSurface->h;
        }
    }
    SDL_FreeSurface(loadedSurface);
    texture = newTexture;
    return texture != NULL;
}

//Metodo para liberar la superficie:
void Texture::free() {
    //En realidad no hay que comprobar si la textura no es nula, ya que SDL_DestroyTexture es seguro ante nulos.
    if(texture != NULL) {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

//Metodo para renderizar en pantalla:
void Texture::render(int x, int y) {
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

//Getters:
int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL no ha podido ser inicializado: " << SDL_GetError() << endl;
        success = false;
    } else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "La calidad del escalado no ha podido ponerse en lineal: " << SDL_GetError() << endl;
        }
        w = SDL_CreateWindow("Trabajando con ViewPorts", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    if(!fooTexture.loadFromFile("assets/lesson10/foo.png")) {
        success = false;
    } else if(!bgTexture.loadFromFile("assets/lesson10/background.png")) {
        success = false;
    }

    return success;
}

void close() {
    //Liberamos las texturas
    fooTexture.free();
    bgTexture.free();

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
                //Pintamos texturas:
                bgTexture.render(0, 0);
                fooTexture.render(240, 190);
                //Presentamos pantalla:
                SDL_RenderPresent(renderer);
            }
        }
    }
    close();
    return 0;
}
