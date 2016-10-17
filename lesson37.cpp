#include <SDL.h>
#include <sstream>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int totalDisplays = 0;
SDL_Rect *displayBounds;

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
        //La id de la ventana:
        unsigned int windowId;
        //La id del display:
        int displayId;

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
    window = SDL_CreateWindow("Multiples displays", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
            displayId = SDL_GetWindowDisplayIndex(window);
            shown = true;
        }
    } else {
        cout << "No ha sido posible crear la ventana: " << endl;
    }

    return window != nullptr && renderer != nullptr;
}

void Window::handleEvent(SDL_Event &e) {
    bool updateCaption = false;
    if(e.type == SDL_WINDOWEVENT && e.window.windowID == windowId) {
        switch(e.window.event) {
            //Vamos a capturar tambien el movimiento de la ventana:
            case SDL_WINDOWEVENT_MOVED:
                displayId = SDL_GetWindowDisplayIndex(window);
                updateCaption = true;
                break;
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
    } else if(e.type == SDL_KEYDOWN) {
        bool cambiarDisplay = false;
        switch(e.key.keysym.sym) {
            case SDLK_RETURN:
                if(e.key.repeat == 0 && SDL_GetModState() & KMOD_ALT) {
                    if(fullscreen) {
                        SDL_SetWindowFullscreen(window, SDL_FALSE);
                        fullscreen = false;
                    } else {
                        SDL_SetWindowFullscreen(window, SDL_TRUE);
                        fullscreen = true;
                        minimized = false;
                    }
                }
                break;
            case SDLK_RIGHT:
                ++displayId;
                cambiarDisplay = true;
                break;
            case SDLK_LEFT:
                --displayId;
                cambiarDisplay = true;
                break;
        }
        if(cambiarDisplay) {
            if(cambiarDisplay < 0) {
                displayId = totalDisplays - 1;
            } else if(cambiarDisplay >= totalDisplays) {
                displayId = 0;
            }
            SDL_SetWindowPosition(window, displayBounds[displayId].x + (displayBounds[displayId].w - width)/2, displayBounds[displayId].y + (displayBounds[displayId].h - height)/2);
        }
    }
    if(updateCaption) {
        stringstream ss;
        ss << "Multiples displays (ID: "<< windowId << " Display: " << displayId << ") - MouseFocus: " << (focusedByMouse?"On":"Off") << " KeyboardFocus: " << (focusedByKeyboard?"On":"Off");
        SDL_SetWindowTitle(window, ss.str().c_str());
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

Window window;

bool init() {
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << SDL_GetError() << endl;
        success = false;
    } else if(!window.init()) {
        cout << SDL_GetError() << endl;
        success = false;
    } else {
        totalDisplays = SDL_GetNumVideoDisplays();
        if(totalDisplays < 2) {
            cout << "Atencion: Solo hay un monitor conectado" << endl;
        }
        displayBounds = new SDL_Rect[totalDisplays];
        for(int i = 0; i < totalDisplays; i++) {
            SDL_GetDisplayBounds(i, &displayBounds[i]);
        }
    }

    return success;
}

void close() {
    window.free();
    delete[] displayBounds;
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if(init()) {
        bool quit = false;
        SDL_Event e;
        while(!quit) {
            while(SDL_PollEvent(&e) != 0) {
                if(e.type == SDL_QUIT) {
                    quit = true;
                } else {
                    window.handleEvent(e);
                }
            }
            //Renderizamos la ventana:
            window.render();
        }
    }
    close();
    return 0;
}
