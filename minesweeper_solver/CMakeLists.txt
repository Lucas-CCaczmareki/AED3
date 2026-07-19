cmake_minimum_required(VERSION 3.16)
project(minesweeper_solver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gera compile_commands.json (ajuda o VSCode/clangd a dar autocomplete certo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------
# CORE — lógica pura, sem Qt. Isso é tudo que roda no VSCode/bash.
# ---------------------------------------------------------------
add_library(core STATIC
    core/src/Board.cpp
    core/src/BoardGenerator.cpp
    core/src/DeterministicSolver.cpp
    core/src/FrontierExtractor.cpp
    core/src/MonteCarloSolver.cpp
    core/src/BruteForceSolver.cpp
    core/src/AutoPlayer.cpp
    
)
target_include_directories(core PUBLIC core/include)

# ---------------------------------------------------------------
# BENCHMARK — executável sem GUI, roda experimentos/métricas
# ---------------------------------------------------------------
add_executable(benchmark benchmark/main2.cpp)
target_link_libraries(benchmark PRIVATE core)

# ---------------------------------------------------------------
# TESTS — testes unitários simples (sem framework externo por ora)
# ---------------------------------------------------------------
add_executable(tests tests/main.cpp)
target_link_libraries(tests PRIVATE core)

# ---------------------------------------------------------------
# AUTOPLAYER — executável sem GUI, roda o solver automaticamente
# ---------------------------------------------------------------
add_executable(autoplayer tests/main_autoplayer.cpp)
target_link_libraries(autoplayer PRIVATE core)

# ---------------------------------------------------------------
# ANALISES ESTATISTICAS EXTRA — convergencia, escala, sensibilidade, calibracao
# ---------------------------------------------------------------
add_executable(k_convergence tests/main_k_convergence.cpp)
target_link_libraries(k_convergence PRIVATE core)

add_executable(scaling_curve tests/main_scaling_curve.cpp)
target_link_libraries(scaling_curve PRIVATE core)

add_executable(sensitivity tests/main_sensitivity.cpp)
target_link_libraries(sensitivity PRIVATE core)

add_executable(calibration tests/main_calibration.cpp)
target_link_libraries(calibration PRIVATE core)

add_executable(benchmark_mc_vs_bf tests/main_benchmark_mc_vs_bf.cpp)
target_link_libraries(benchmark_mc_vs_bf PRIVATE core)

# ---------------------------------------------------------------
# GUI (Qt) — só ativa se o Qt6 for encontrado no sistema.
# Não quebra o build no VSCode se o Qt não estiver instalado ainda.
# ---------------------------------------------------------------
find_package(Qt6 QUIET COMPONENTS Widgets)

if(Qt6_FOUND)
    message(STATUS "Qt6 encontrado — GUI habilitada.")

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    add_executable(minesweeper_gui
        gui/main.cpp
        gui/mainwindow.cpp
        gui/BoardWidget.cpp
        gui/ControlsPanel.cpp
        gui/mainwindow.h
        gui/mainwindow.ui
    )
    target_link_libraries(minesweeper_gui PRIVATE core Qt6::Widgets)
else()
    message(STATUS "Qt6 NAO encontrado — GUI desabilitada (core/benchmark/tests ainda funcionam normalmente).")
endif()