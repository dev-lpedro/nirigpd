#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdlib>
#include <magic_enum/magic_enum.hpp>
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include <SDL3/SDL_timer.h>

#include "../include/config.hpp"


bool checar_combo(SDL_Gamepad* gamepad, const vector<int>& combo);
void tratar_erro_nao_configurado(const SDL_Event& event, int deadzone_global);
string obter_nome_do_input(InputType tipo_de_evento, const InputKey& ik);
void executar_loop_hold(const string& acao, InputType tipo_de_evento, const InputKey& ik, Sint32 delay_ms, int deadzone);
