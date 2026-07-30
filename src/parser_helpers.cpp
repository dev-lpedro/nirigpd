#include "../include/parser.hpp"
#include <magic_enum/magic_enum.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

string ler_arquivo(const string& arq) {
    ifstream ifs(arq.c_str());
    string str((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    return str;
}

// 1. TRADUTOR DE BOTÕES DIGITAIS
SDL_GamepadButton traduzir_botao(string_view nome_botao) {
    if (nome_botao == "BTN_SOUTH")  return SDL_GAMEPAD_BUTTON_SOUTH;
    if (nome_botao == "BTN_EAST")   return SDL_GAMEPAD_BUTTON_EAST;
    if (nome_botao == "BTN_WEST")   return SDL_GAMEPAD_BUTTON_WEST;
    if (nome_botao == "BTN_NORTH")  return SDL_GAMEPAD_BUTTON_NORTH;
    
    if (nome_botao == "BTN_TL")     return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
    if (nome_botao == "BTN_TR")     return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
    
    if (nome_botao == "BTN_START")  return SDL_GAMEPAD_BUTTON_START;
    if (nome_botao == "BTN_SELECT") return SDL_GAMEPAD_BUTTON_BACK;
    if (nome_botao == "BTN_MODE")   return SDL_GAMEPAD_BUTTON_GUIDE;
    
    if (nome_botao == "BTN_THUMBL") return SDL_GAMEPAD_BUTTON_LEFT_STICK;
    if (nome_botao == "BTN_THUMBR") return SDL_GAMEPAD_BUTTON_RIGHT_STICK;
    
    if (nome_botao == "DPAD_UP")    return SDL_GAMEPAD_BUTTON_DPAD_UP;
    if (nome_botao == "DPAD_DOWN")  return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
    if (nome_botao == "DPAD_LEFT")  return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
    if (nome_botao == "DPAD_RIGHT") return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
    
    return SDL_GAMEPAD_BUTTON_INVALID;
}

// 2. TRADUTOR DE EIXOS E GATILHOS (A direção volta por referência para montar a chave depois)
SDL_GamepadAxis traduzir_eixo(string_view nome, int& direcao) {
    // Gatilhos (Sempre direção positiva: 0 até 32767)
    if (nome == "BTN_TL2") { direcao = 1; return SDL_GAMEPAD_AXIS_LEFT_TRIGGER; }
    if (nome == "BTN_TR2") { direcao = 1; return SDL_GAMEPAD_AXIS_RIGHT_TRIGGER; }
    
    // Analógico Esquerdo
    if (nome == "L_STICK_UP")    { direcao = -1; return SDL_GAMEPAD_AXIS_LEFTY; }
    if (nome == "L_STICK_DOWN")  { direcao = 1;  return SDL_GAMEPAD_AXIS_LEFTY; }
    if (nome == "L_STICK_LEFT")  { direcao = -1; return SDL_GAMEPAD_AXIS_LEFTX; }
    if (nome == "L_STICK_RIGHT") { direcao = 1;  return SDL_GAMEPAD_AXIS_LEFTX; }
    
    // Analógico Direito
    if (nome == "R_STICK_UP")    { direcao = -1; return SDL_GAMEPAD_AXIS_RIGHTY; }
    if (nome == "R_STICK_DOWN")  { direcao = 1;  return SDL_GAMEPAD_AXIS_RIGHTY; }
    if (nome == "R_STICK_LEFT")  { direcao = -1; return SDL_GAMEPAD_AXIS_RIGHTX; }
    if (nome == "R_STICK_RIGHT") { direcao = 1;  return SDL_GAMEPAD_AXIS_RIGHTX; }
    
    return SDL_GAMEPAD_AXIS_INVALID;
}

unordered_set<TriggerType> parse_trigger_settings(string_view val) {
    unordered_set<TriggerType> ts;

    // Fazemos a varredura por substrings
    if (val.find("press") != string_view::npos) {
        ts.insert(TriggerType::press);
    }
    if (val.find("hold") != string_view::npos) {
        ts.insert(TriggerType::hold);
    }

    // Se a propriedade "on" foi declarada mas veio vazia ou inválida, mantemos press por segurança
    if (ts.empty()) {
        ts.insert(TriggerType::press);
    }
    return ts;
}

void processar_inicio_no(string_view nome_no, ParserState& state) {
    if (state.nome_no_atual == "hold_bindings") {
        state.escopo_atual = "hold";
    } else if (state.nome_no_atual == "toggle_bindings") {
        state.escopo_atual = "toggle";
    } else if (state.escopo_atual != "global") {
        // Reseta os estados
        state.botao_atual = SDL_GAMEPAD_BUTTON_INVALID;
        state.eixo_atual = SDL_GAMEPAD_AXIS_INVALID;
        state.bind_temp = BindingConfig();
        
        // Tenta ler como Botão
        state.botao_atual = traduzir_botao(nome_no);
        
        // Se não for botão, tenta ler como Eixo (Sticks ou Triggers)
        if (state.botao_atual == SDL_GAMEPAD_BUTTON_INVALID) {
            state.eixo_atual = traduzir_eixo(nome_no, state.direcao_atual);
            
            if (state.eixo_atual != SDL_GAMEPAD_AXIS_INVALID) {
                state.bind_temp.eixo = new AxisConfig();
                //adicionar a deadzone propria depois
            }
        }
    }
}

void processar_argumento(string_view valor, ParserState& state, Config& cfg) {
    //Lembrar de possibilitar o uso dos eixos alem dos botões (necessario para usar os triggers)
    if (state.escopo_atual == "global") {
        if (state.nome_no_atual == "hold_combo") {
            cfg.combo[BindingMode::hold].push_back(traduzir_botao(valor));
        } else if (state.nome_no_atual == "toggle_combo") {
            cfg.combo[BindingMode::toggle].push_back(traduzir_botao(valor));
        } else if (state.nome_no_atual == "deadzone") {
            cfg.deadzone = stoi(string(valor));
        }
    } else if (state.botao_atual != SDL_GAMEPAD_BUTTON_INVALID || state.eixo_atual != SDL_GAMEPAD_AXIS_INVALID) {
        state.bind_temp.action = string(valor);
    }
}

void processar_propriedade(string_view prop_nome, string_view prop_valor, ParserState& state) {
    if (state.botao_atual != SDL_GAMEPAD_BUTTON_INVALID || state.eixo_atual != SDL_GAMEPAD_AXIS_INVALID) {
        if (prop_nome == "on") {
            state.bind_temp.trigger = parse_trigger_settings(prop_valor);
        } 
        else if (prop_nome == "delay_ms") {
            state.bind_temp.delay_ms = stoi(string(prop_valor));
        }
        else if (prop_nome == "mod") {
            //state.bind_temp.mod_button = traduzir_botao(prop_valor);
        }
        else if (prop_nome == "deadzone") {
            int dz_custom = stoi(string(prop_valor));
            if (state.bind_temp.eixo) state.bind_temp.eixo->deadzone_proprio = dz_custom;
        }
    }
}

void processar_fim_no(ParserState& state, Config& cfg) {
    if (state.nome_no_atual == "hold_bindings" || state.nome_no_atual == "toggle_bindings") {
        state.escopo_atual = "global"; 
    } 
    else {
        // 1. Converte a string do escopo_atual para o seu enum BindingMode
        BindingMode modo_atual = magic_enum::enum_cast<BindingMode>(state.escopo_atual, magic_enum::case_insensitive).value_or(BindingMode::global);

        if (state.botao_atual != SDL_GAMEPAD_BUTTON_INVALID) {
            // 2. Monta a chave unificada para o BOTÃO
            InputKey chave;
            chave.tipo = InputType::button;
            chave.id = static_cast<int>(state.botao_atual);
            chave.direcao = 0; // Botão não usa direção

            // 3. Salva direto no mapa mestre!
            cfg.bindings[modo_atual][chave] = state.bind_temp;
        }
        else if (state.eixo_atual != SDL_GAMEPAD_AXIS_INVALID) {
            // 2. Monta a chave unificada para o EIXO
            InputKey chave;
            chave.tipo = InputType::axis;
            chave.id = static_cast<int>(state.eixo_atual);
            chave.direcao = state.direcao_atual; // Usa o 1 ou -1 que você já tinha

            // 3. Salva direto no mapa mestre!
            cfg.bindings[modo_atual][chave] = state.bind_temp;
        }
    }
}

void print_config_debug(const Config& cfg) {
    cout << "\n========================================\n";
    cout << "        CONFIGURAÇÕES CARREGADAS        \n";
    cout << "========================================\n\n";

    cout << "[CONFIGURAÇÕES GLOBAIS]\n";
    cout << "Deadzone Global: " << cfg.deadzone << "\n\n";

    cout << "[COMBOS REGISTRADOS]\n";
    if (cfg.combo.empty()) {
        cout << "  Nenhum combo configurado.\n";
    } else {
        for (const auto& [modo, botoes] : cfg.combo) {
            cout << "  Modo " << magic_enum::enum_name(modo) << ": [ ";
            for (int btn : botoes) {
                // Converte o int de volta para o enum da SDL para o magic_enum ler
                cout << magic_enum::enum_name(static_cast<SDL_GamepadButton>(btn)) << " ";
            }
            cout << "]\n";
        }
    }
    cout << "\n";

    cout << "[MAPEAMENTOS DE ENTRADA (BINDS)]\n";
    if (cfg.bindings.empty()) {
        cout << "  Nenhuma bind configurada.\n";
    } else {
        for (const auto& [modo, mapa_binds] : cfg.bindings) {
            cout << "--- Escopo/Modo: " << magic_enum::enum_name(modo) << " ---\n";
            
            for (const auto& [chave, bind] : mapa_binds) {
                cout << "  Entrada: ";
                
                // Printa o botão ou o eixo
                if (chave.tipo == InputType::button) {
                    cout << "[Botão] " << magic_enum::enum_name(static_cast<SDL_GamepadButton>(chave.id));
                } 
                else if (chave.tipo == InputType::axis) {
                    cout << "[Eixo] " << magic_enum::enum_name(static_cast<SDL_GamepadAxis>(chave.id));
                    cout << " (Direção: " << (chave.direcao > 0 ? "+" : "-") << ")";
                }

                cout << "\n    -> Ação: \"" << bind.action << "\"";
                
                // Exibe os elementos ativos do set iterando dinamicamente
                cout << " | Gatilhos: [";
                bool primeiro = true;
                for (const auto& t : bind.trigger) {
                    if (!primeiro) cout << " ";
                    cout << magic_enum::enum_name(t);
                    primeiro = false;
                }
                cout << "]";
                          
                if (bind.delay_ms > 0) {
                    cout << " | Delay: " << bind.delay_ms << "ms";
                }
                
                if (bind.eixo && bind.eixo->deadzone_proprio != -1) {
                    cout << " | Deadzone Custom: " << bind.eixo->deadzone_proprio;
                }
                
                cout << "\n\n";
            }
        }
    }
    cout << "========================================\n\n";
}