# Déclare la version minimale de CMake
cmake_minimum_required(VERSION 3.10)

# Nom du projet
project(MiniShell VERSION 1.0 LANGUAGES C)

# Ajoute les options de compilation
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Ajoute les fichiers source
set(SOURCES
    src/main.c
    src/parser/parser.c
    src/executor/executor.c
    src/builtins/builtins.c
    src/history/history.c
)

# Ajoute les fichiers d'en-tête
set(HEADERS
    src/typedef.h
    src/parser/parser.h
    src/executor/executor.h
    src/builtins/builtins.h
    src/history/history.h
)

# Cible pour créer l'exécutable
add_executable(minishell ${SOURCES} ${HEADERS})

# Ajoute des options spécifiques (optionnel)
target_include_directories(minishell PRIVATE ${CMAKE_SOURCE_DIR})

# Ajout de commandes Doxygen pour la documentation (optionnel)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/docs)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Génération de la documentation avec Doxygen"
        VERBATIM
    )
endif()

# Affiche un message de succès après configuration
message(STATUS "Configuration réussie avec CMake. Utilisez 'make' pour compiler le projet.")
