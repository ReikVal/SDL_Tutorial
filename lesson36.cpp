#include <SDL.h>
#include <sstream>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Wrapper para la ventana:
class Window {
    public:
        //Crea la ventana:
        bool init();

        //Handler de los eventos de ventana:
        void handleEvent(SDL_Event &e);

        //Focus en la ventana:
        void focus();

        //Y su renderizado:
        void render();

        //Libera recursos:
        void free();

        //Getters:
        int getWidth();
        int getHeight();
        bool isFocusedByMouse();
        bool isFocusedByKeyboard();
        bool isMinimized();
        bool isShown();
    private:
        //La ventana en si:
        SDL_Window *window;
        //Y el renderer asociado a ella:
        SDL_Renderer *renderer;
        //Y su id:
        unsigned int windowId;

        //Dimensiones y propiedades:
        int width{0};
        int height{0};
        bool focusedByMouse{false};
        bool focusedByKeyboard{false};
        bool minimized{false};
        bool fullscreen{false};
        bool shown{false};
};

bool Window::init() {
    window = SDL_CreateWindow("Multiples ventanas", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window != nullptr) {
        focusedByMouse = true;
        focusedByKeyboard = true;
        width = SCREEN_WIDTH;
        height = SCREEN_HEIGHT;
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(renderer == nullptr) {
            cout << "No se ha podido crear el renderer: " << endl;
            SDL_DestroyWindow(window);
            window = nullptr;
        } else {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            windowId = SDL_GetWindowID(window);
            shown = true;
        }
    }
    return window != nullptr && renderer != nullptr;
}

void Window::handleEvent(SDL_Event &e) {
    if(e.type == SDL_WINDOWEVENT && e.window.windowID == windowId) {
        bool updateCaption = false;
        switch(e.window.event) {
            //Mostrado u ocultamiento de la ventana:
            case SDL_WINDOWEVENT_SHOWN:
                shown = true;
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                shown = false;
                break;
            //Cambio de tamaño:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                width = e.window.data1;
                height = e.window.data2;
                SDL_RenderPresent(renderer);
                break;
            //Se vuelve a ver:
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_RenderPresent(renderer);
                break;
            //Eventos de raton:
            case SDL_WINDOWEVENT_ENTER:
                focusedByMouse = true;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_LEAVE:
                focusedByMouse = false;
                updateCaption = true;
                break;
            //Eventos de teclado:
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                focusedByKeyboard = true;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                focusedByKeyboard = false;
                updateCaption = true;
                break;
            //Minimizado, maximizado y restaurado:
            case SDL_WINDOWEVENT_MINIMIZED:
                minimized = true;
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_RESTORED:
                minimized = false;
                break;
            case SDL_WINDOWEVENT_CLOSE:
                SDL_HideWindow(window);
                break;
        }
        if(updateCaption) {
            stringstream ss;
            ss << "Multiples ventanas (ID: "<< windowId << ") - MouseFocus: " << (focusedByMouse?"On":"Off") << " KeyboardFocus: " << (focusedByKeyboard?"On":"Off");
            SDL_SetWindowTitle(window, ss.str().c_str());
        }
    } else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && e.key.repeat == 0 && SDL_GetModState() & KMOD_ALT) {
        if(fullscreen) {
            SDL_SetWindowFullscreen(window, SDL_FALSE);
            fullscreen = false;
        } else {
            SDL_SetWindowFullscreen(window, SDL_TRUE);
            fullscreen = true;
            minimized = false;
        }
    }
}

void Window::focus() {
    if(!shown) {
        SDL_ShowWindow(window);
    }

    SDL_RaiseWindow(window);
}

void Window::render() {
    if(!minimized) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }
}

void Window::free() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

int Window::getWidth() {
    return width;
}

int Window::getHeight() {
    return height;
}

bool Window::isFocusedByKeyboard() {
    return focusedByKeyboard;
}

bool Window::isFocusedByMouse() {
    return focusedByMouse;
}

bool Window::isMinimized() {
    return minimized;
}

bool Window::isShown() {
    return shown;
}

const int TOTAL_WINDOWS = 3;
Window window[TOTAL_WINDOWS];

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else if(!window[0].init()) {
        cout << SDL_GetError() << endl;
        success = false;
    }

    return success;
}

void close() {
    for(int i = 0; i < TOTAL_WINDOWS; i++) {
        window[i].free();
    }

    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if(init()) {
        for(int i = 1; i < TOTAL_WINDOWS; i++) {
            window[i].init();
        }
        bool quit = false;
        SDL_Event e;
        while(!quit) {
            while(SDL_PollEvent(&e) != 0) {
                if(e.type == SDL_QUIT) {
                    quit = true;
                } else if(e.type == SDL_KEYDOWN) {
                    //Cambiamos el focus a la ventana que pulsemos la tecla:
                    switch(e.key.keysym.sym) {
                        case SDLK_1:
                            window[0].focus();
                            break;
                        case SDLK_2:
                            window[1].focus();
                            break;
                        case SDLK_3:
                            window[2].focus();
                            break;
                    }
                } else {
                    for(int i = 0; i < TOTAL_WINDOWS; i++) {
                        window[i].handleEvent(e);
                    }
                }
            }
            //Renderizamos todas las ventanas:
            for(int i = 0; i < TOTAL_WINDOWS; i++) {
                window[i].render();
            }

            //Comprobamos si estan todas ocultas para cerrar el programa:
            bool todasOcultas = true;
            for(int i = 0; i < TOTAL_WINDOWS; i++) {
                if(window[i].isShown()) {
                    todasOcultas = false;
                }
            }

            if(todasOcultas) {
                quit = true;
            }
        }
    }
    close();
    return 0;
}
