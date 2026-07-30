#pragma once
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_stdinc.h>
#include <string>
#include <map>
#include <unordered_set>
#include <vector>


using namespace std;

enum class TriggerType {
    press,
    hold
};

enum class BindingMode {
    toggle,
    hold,
    global
};

enum class InputType {
    button,
    axis,
    invalid
};

struct InputKey {
    InputType tipo;
    int id;         // Guarda o SDL_GamepadButton ou SDL_GamepadAxis (como inteiro)
    int direcao;    // 0 para botões, 1 (positivo) ou -1 (negativo) para eixos

    // O std::map exige o operator< para saber como organizar as chaves internamente
    bool operator<(const InputKey& outra) const {
        return std::tie(tipo, id, direcao) < std::tie(outra.tipo, outra.id, outra.direcao);
    }
};

// Guarda as configurações específicas de comportamento físico de um Eixo
struct AxisConfig {
    int valor;
    int deadzone_proprio = -1;         // -1 significa "usar deadzone global"
};

// O que deve acontecer quando uma entrada (botão ou eixo) é acionada
struct BindingConfig {
    string action;
    unordered_set<TriggerType> trigger = {TriggerType::press};              // press, hold
    Sint32 delay_ms = 0;
    
    // Ponteiros opcionais para quando a bind vier de um analógico ou gatilho
    AxisConfig* eixo = nullptr;
};

struct Config {
    int deadzone = 15000;
    map<BindingMode, vector<int>> combo;
    
    // --- MAPAS DE BOTÕES (Digitais: D-PAD, A/B/X/Y, Ombros L1/R1, Cliques L3/R3) ---

    map<BindingMode, map<InputKey, BindingConfig>> bindings;

};