#pragma once
#include "config.hpp"
#include <SDL3/SDL_gamepad.h>
#include <string>
#include <string_view>
#include <vector>
#include <kdl/parser.h>

// Estrutura para encapsular o estado do parser sem sujar a assinatura das funções
struct ParserState {
    std::string escopo_atual = "global"; 
    std::string nome_no_atual = "";
    SDL_GamepadButton botao_atual = SDL_GAMEPAD_BUTTON_INVALID;
    SDL_GamepadAxis eixo_atual = SDL_GAMEPAD_AXIS_INVALID;
    int direcao_atual = 0;
    BindingConfig bind_temp;
};

// Funções Principais
Config parser(std::string config_path);
void print_config_debug(const Config& cfg);

// Auxiliares de Tradução e Leitura
std::string ler_arquivo(const std::string& arq);
SDL_GamepadButton traduzir_botao(std::string_view nome_botao);
SDL_GamepadAxis traduzir_eixo(std::string_view nome, int& direcao);

// Auxiliares de Processamento de Eventos KDL
void processar_inicio_no(std::string_view nome_no, ParserState& state);
void processar_argumento(std::string_view valor, ParserState& state, Config& cfg);
void processar_propriedade(std::string_view prop_nome, std::string_view prop_valor, ParserState& state);
void processar_fim_no(ParserState& state, Config& cfg);
unordered_set<TriggerType> parse_trigger_settings(std::string_view val);