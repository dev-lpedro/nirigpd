#pragma once

#include "config.hpp"
#include <iostream>
#include <cstdlib>
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_stdinc.h>
#include <string>
#include <algorithm> // Necessário para o std::find
#include <map>
#include <cmath>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdlib>
#include <magic_enum/magic_enum.hpp>
#include <utility>
#include <vector>
#include <iostream>


struct Config;

SDL_Gamepad* inicializar_sdl();

void rodar_loop(SDL_Gamepad* gamepad, const Config* config);