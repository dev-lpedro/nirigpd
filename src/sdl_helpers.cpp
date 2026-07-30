#include "../include/sdl_helpers.hpp"

using namespace std;

// Função para saber se um botão específico faz parte do combo
bool eh_botao_de_combo(int botao, const vector<int>& combo) {
    return std::find(combo.begin(), combo.end(), botao) != combo.end();
}

// Função auxiliar exclusiva deste arquivo para checar o combo
bool checar_combo(SDL_Gamepad* gamepad, const vector<int>& combo) {
    if (combo.empty() || gamepad == nullptr) return false;
    
    // Se achar qualquer botão do combo que NÃO esteja apertado, retorna falso
    for (int btn : combo) {
        if (!SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)btn)) {
            return false; 
        }
    }
    return true; // Todos os botões do combo estão afundados!
}

// Trata e exibe os logs de inputs que não possuem binds configuradas
void tratar_erro_nao_configurado(const SDL_Event& event, int deadzone_global) {
    if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION && abs(event.gaxis.value) > deadzone_global) {
        // Converte o ID numérico do eixo para a representação em enum da SDL3
        SDL_GamepadAxis axis_enum = static_cast<SDL_GamepadAxis>(event.gaxis.axis);
        
        // Pega o nome textual que a SDL dá para esse eixo
        const char* axis_name = SDL_GetGamepadStringForAxis(axis_enum);
        
        cout << "axis " << (axis_name ? axis_name : "desconhecido") << " [" << (event.gaxis.value < 0 ? "-]" : "+]") <<" não configurado " << endl;
    } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
        // Converte o ID numérico do botão para a representação em enum da SDL3
        SDL_GamepadButton button_enum = static_cast<SDL_GamepadButton>(event.gbutton.button);
        
        // Pega o nome textual que a SDL dá para esse botão
        const char* button_name = SDL_GetGamepadStringForButton(button_enum);
        
        cout << "botao " << (button_name ? button_name : "desconhecido") << " não configurado" << endl;
    }
}

// Resolve o nome legível da entrada física atual (Botão ou Eixo)
string obter_nome_do_input(InputType tipo_de_evento, const InputKey& ik) {
    string nome_do_input = "desconhecido";
    
    if (tipo_de_evento == InputType::button) {
        const char* name = SDL_GetGamepadStringForButton(static_cast<SDL_GamepadButton>(ik.id));
        if (name) nome_do_input = name;
    } 
    else if (tipo_de_evento == InputType::axis) {
        const char* name = SDL_GetGamepadStringForAxis(static_cast<SDL_GamepadAxis>(ik.id));
        if (name) {
            nome_do_input = name;
            // Opcional: Adiciona a direção (+ ou -) para ficar mais claro no log dos analógicos
            if (ik.direcao == 1) nome_do_input += " (+)";
            else if (ik.direcao == -1) nome_do_input += " (-)";
        }
    }
    return nome_do_input;
}

// Executa o sub-loop dinâmico temporal para repetição contínua (Hold Mode)
void executar_loop_hold(const string& acao, InputType tipo_de_evento, const InputKey& ik, Sint32 delay_ms, int deadzone) {
    bool ainda_segurando = true;
    
    // Proteção contra delay zero ou negativo (evita travar a CPU)
    if (delay_ms < 1) delay_ms = 1; 

    // Define exatamente quando deve ser o próximo disparo do comando
    Uint64 proximo_disparo = SDL_GetTicks() + delay_ms;

    while (ainda_segurando) {
        Uint64 agora = SDL_GetTicks();
        Sint32 tempo_restante = 0;

        if (agora >= proximo_disparo) {
            system(("niri msg action " + acao).c_str());
            proximo_disparo = agora + delay_ms;
            tempo_restante = delay_ms;
        } 
        else {
            tempo_restante = static_cast<Sint32>(proximo_disparo - agora);
        }

        SDL_Event ev;
        
        if (SDL_WaitEventTimeout(&ev, tempo_restante)) {
            
            // --- PROTEÇÃO CRÍTICA CONTRA LOOP INFINITO ---
            // Se o programa for fechado ou o controle desconectar, pare IMEDIATAMENTE!
            if (ev.type == SDL_EVENT_QUIT || ev.type == SDL_EVENT_GAMEPAD_REMOVED) {
                ainda_segurando = false;
                
                // Devolve o QUIT para a fila para o while principal ler depois
                if (ev.type == SDL_EVENT_QUIT) {
                    SDL_PushEvent(&ev);
                }
                break;
            }

            // Caso A: Se for o botão que estávamos segurando e ele foi SOLTO
            if (tipo_de_evento == InputType::button && 
                ev.type == SDL_EVENT_GAMEPAD_BUTTON_UP && 
                ev.gbutton.button == ik.id) {
                ainda_segurando = false;
            }
            
            // Caso B: Se for o analógico/eixo e ele voltou para dentro da deadzone
            else if (tipo_de_evento == InputType::axis && 
                    ev.type == SDL_EVENT_GAMEPAD_AXIS_MOTION && 
                    ev.gaxis.axis == ik.id) {
                
                Sint16 valor = ev.gaxis.value;
                if (ik.direcao == 1 && valor < deadzone) {
                    ainda_segurando = false;
                } else if (ik.direcao == -1 && valor > -deadzone) {
                    ainda_segurando = false;
                } else if (ik.direcao == 0 && abs(valor) < deadzone) {
                    ainda_segurando = false;
                }
            }
        }
    }

    // 4. Limpa APENAS os eventos de gamepad acumulados (eixos e botões)
    // Isso evita apagar eventos do sistema como o SDL_EVENT_QUIT
    SDL_FlushEvents(SDL_EVENT_GAMEPAD_AXIS_MOTION, SDL_EVENT_GAMEPAD_BUTTON_UP);
}