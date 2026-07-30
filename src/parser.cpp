#include "../include/parser.hpp"
#include <magic_enum/magic_enum.hpp>
#include <iostream>

using namespace std;

Config parser(string config_path) {
    string buffer = ler_arquivo(config_path);
    kdl_parser* parser = kdl_create_string_parser(kdl_str{buffer.c_str(), buffer.size()}, KDL_DEFAULTS);
    
    Config cfg;
    ParserState state;

    kdl_event_data* evento = kdl_parser_next_event(parser);

    while (evento->event != KDL_EVENT_EOF) {
        switch (evento->event) {
            
            case KDL_EVENT_START_NODE: {
                string_view nome_no(evento->name.data, evento->name.len);
                state.nome_no_atual = string(nome_no);
                processar_inicio_no(nome_no, state);
                break;
            }
            
            case KDL_EVENT_ARGUMENT: {
                if (evento->value.type == KDL_TYPE_STRING) {
                    string_view valor(evento->value.string.data, evento->value.string.len);
                    processar_argumento(valor, state, cfg);
                }
                break;
            }
            
            case KDL_EVENT_PROPERTY: {
                string_view prop_nome(evento->name.data, evento->name.len);
                if (evento->value.type == KDL_TYPE_STRING) {
                    string_view prop_valor(evento->value.string.data, evento->value.string.len);
                    processar_propriedade(prop_nome, prop_valor, state);
                }
                break;
            }
            
            case KDL_EVENT_END_NODE: {
                processar_fim_no(state, cfg);
                break;
            }
            
            default:
                break;
        }
        
        // Pega o próximo evento e avança
        evento = kdl_parser_next_event(parser);
    }

    kdl_destroy_parser(parser);
    
    return cfg;
}