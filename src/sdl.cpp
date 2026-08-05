#include "../include/sdl.hpp"
#include "../include/config.hpp"
#include "../include/sdl_helpers.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>

using namespace std;

SDL_Gamepad* procurar_controle() {
    int count = 0;
    SDL_JoystickID* id = SDL_GetGamepads(&count);
    SDL_Gamepad* my_gamepad = nullptr;

    if (count > 0) {
        my_gamepad = SDL_OpenGamepad(id[0]);
        cout << "Controle conectado!" << endl;
        cout << "Name: " << SDL_GetGamepadName(my_gamepad) << endl;
        cout << "Path: " << SDL_GetGamepadPath(my_gamepad) << endl;
    } else {
        cout << "Nenhum controle encontrado." << endl;
    }

    SDL_free(id);
    return my_gamepad;
}

// 1. FUNÇÃO DE INICIALIZAÇÃO
SDL_Gamepad* inicializar_sdl() {
    SDL_Init(SDL_INIT_GAMEPAD);
    return procurar_controle();
}

BindingMode checar_modo(SDL_Gamepad* gamepad , const Config* config) {

    for (BindingMode valor : magic_enum::enum_values<BindingMode>()) {

        auto it = config->combo.find(valor);
    
        if (it != config->combo.end()) {
            const std::vector<int>& combo = it->second;

            if (checar_combo(gamepad, combo)) {
                return valor;
            }        
        }       
    }

    return BindingMode::global;
}

InputType checar_button_ou_axis(const SDL_Event event) {
    if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
        return InputType::button;
    } else if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
        return InputType::axis;
    }
    return InputType::invalid;
}

void rodar_loop(SDL_Gamepad* gamepad , const Config* config) {
    SDL_Event event;
    bool is_toggle_mode = false;

    while (SDL_WaitEvent(&event) && event.type != SDL_EVENT_QUIT) {

        if (event.type == SDL_EVENT_GAMEPAD_ADDED && gamepad == nullptr) {
            gamepad = procurar_controle();
            continue;
        } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED && event.gdevice.which == SDL_GetGamepadID(gamepad)) {
            cout << "Controle desconectado!" << endl;
            SDL_CloseGamepad(gamepad);
            gamepad = nullptr;
            continue;
        }else if (gamepad == nullptr || !(event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN || event.type == SDL_EVENT_GAMEPAD_BUTTON_UP || event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION)) {
            continue;
        }

        BindingMode modo = checar_modo(gamepad, config);
        if (modo == BindingMode::toggle && event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
            is_toggle_mode = !is_toggle_mode;
            cout << "Modo trocado para " << is_toggle_mode << endl;
        }

        if (is_toggle_mode) {
            modo = BindingMode::toggle;
        }

        if (!(modo == BindingMode::hold || modo == BindingMode::toggle)) continue;

        InputType tipo_de_evento = checar_button_ou_axis(event);
        if (tipo_de_evento == InputType::invalid) {
            continue;
        }

        //descobre qual acao tomar
        string acao;
        
        InputKey ik;
        ik.tipo = tipo_de_evento; 
        if (tipo_de_evento == InputType::button) {
            ik.id = event.gbutton.button;
            ik.direcao = 0;
        } else if (tipo_de_evento == InputType::axis) {
            ik.id = event.gaxis.axis;
            if (event.gaxis.value > 0){
                ik.direcao = 1;
            } else if (event.gaxis.value < 0) {
                ik.direcao = -1;
            } else {
                continue;
            }
        }
        
        auto mapa = config->bindings.find(modo);

        if (mapa != config->bindings.end()) {
            auto it = mapa->second.find(ik);
            if (it != mapa->second.end()) {
                acao = it->second.action;
            } else {
                tratar_erro_nao_configurado(event, config->deadzone);
                continue;
            }
            Sint32 delay_ms = it->second.delay_ms;
            const auto& trigger = it->second.trigger;
            AxisConfig* eixo = it->second.eixo;
            
            int deadzone = -1;
            if (eixo != nullptr) {
                //deadzone recebe deadzone global caso o deadzone da propria bind seja -1, caso contrario deadzone recebe deadzone global
                deadzone = 
                eixo->deadzone_proprio < 0 
                ? config->deadzone : eixo->deadzone_proprio; 

                if (abs(event.gaxis.value) < deadzone) {
                    continue;
                }
            }

            if (!acao.empty()) {
                // 1. Descobre o nome legível do input (seja botão ou eixo)
                string nome_do_input = obter_nome_do_input(tipo_de_evento, ik);

                // 2. Imprime tudo em um único fluxo limpo
                cout 
                << "Evento do tipo " << magic_enum::enum_name(tipo_de_evento) 
                << " no modo " << magic_enum::enum_name(modo) 
                << " | Input: " << nome_do_input
                << " -> Fazendo a acao de: " << acao << endl;

                // 3. Executa o comando
                if (trigger.find(TriggerType::press) != trigger.end()){
                    system(("niri msg action " + acao).c_str());    
                }

                if (trigger.find(TriggerType::hold) != trigger.end()) {
                    executar_loop_hold(acao, tipo_de_evento, ik, delay_ms, deadzone);
                }
            }
        }       
    }
}