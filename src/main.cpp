#include <SDL3/SDL.h>
#include <iostream>
#include <string>

#include "../include/config.hpp"
#include "../include/parser.hpp"
#include "../include/sdl.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    string config_path = (argc > 1) ? argv[1] : "gamepad.kdl";

    // 1. Carrega as configurações
    Config config = parser(config_path);

    // 2. Inicializa os sistemas
    SDL_Gamepad* my_gamepad = inicializar_sdl();
    print_config_debug(config);
    
    // 3. Roda o programa
    rodar_loop(my_gamepad, &config);

    // 4. Limpa e fecha (Só roda quando o loop quebrar)
    if (my_gamepad) {
        SDL_CloseGamepad(my_gamepad);
    }
    SDL_Quit();

    return 0;
}