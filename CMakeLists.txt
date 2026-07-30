cmake_minimum_required(VERSION 3.15)
project(nirigpd CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Busca SDL3 no sistema
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

# Busca magic_enum no sistema
find_package(magic_enum CONFIG REQUIRED)

# Baixa a ckdl
include(FetchContent)
FetchContent_Declare(
    ckdl
    GIT_REPOSITORY https://github.com/tjol/ckdl.git
    GIT_TAG        main
)
set(BUILD_TESTS OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(ckdl)

# Avisa ao compilador onde buscar os nossos próprios .hpp
include_directories(include)

# Compila todos os arquivos .cpp da pasta src/ num único executável
add_executable(nirigpd 
    src/main.cpp
    src/sdl.cpp
    src/sdl_helpers.cpp       # <--- Novo helper do SDL adicionado aqui
    src/parser.cpp
    src/parser_helpers.cpp    # <--- Novo helper do Parser adicionado aqui
)

target_include_directories(nirigpd PRIVATE 
    ${SDL3_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/_deps/ckdl-src/include
    ${CMAKE_BINARY_DIR}/_deps/ckdl-src/include/kdl
)

# Linka todas as bibliotecas ao seu executável
target_link_libraries(nirigpd PRIVATE 
    ${SDL3_LIBRARIES} 
    kdl 
    magic_enum::magic_enum
)