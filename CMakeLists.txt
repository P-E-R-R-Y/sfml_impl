#----------- general -----------

cmake_minimum_required(VERSION 3.24)
project(sfml_impl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#----------- project -----------

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(CMakeUtils REQUIRED)
include(${cmakeutils_SOURCE_DIR}/ToolVar.cmake)

# FindSfml force BUILD_SHARED_LIBS ET CMAKE_POSITION_INDEPENDENT_CODE :
# on les retire pour que chaque dependance decide, et on les remet apres.
cmake_var_extract(BUILD_SHARED_LIBS)
cmake_var_extract(CMAKE_POSITION_INDEPENDENT_CODE)

find_package(System REQUIRED)
find_package(modulemanager REQUIRED)
find_package(igraphic REQUIRED)
find_package(iaudio REQUIRED)
find_package(Sfml REQUIRED)

cmake_var_restore(BUILD_SHARED_LIBS)
cmake_var_restore(CMAKE_POSITION_INDEPENDENT_CODE)

set(SOURCE_FILES sources/sfml.cpp)

# Deux facons de recuperer ce vendor, deux cibles explicites :
#
#   sfml_impl         SHARED  sfml_impl.dylib   -> a dlopen (ModuleManager)
#   sfml_impl_static  STATIC  libsfml_impl.a    -> a linker en dur
#
# Le type n'est VOLONTAIREMENT pas gouverne par BUILD_SHARED_LIBS. La raison
# d'etre du plugin est d'etre charge a chaud : si le drapeau global le
# transformait en .a, Load() echouerait au runtime avec un message obscur,
# et l'exemple dynamique comme les tests casseraient sans rien dire. Le
# consommateur choisit par le NOM de la cible, pas par un drapeau global.
add_library(${PROJECT_NAME}        SHARED ${SOURCE_FILES})
add_library(${PROJECT_NAME}_static STATIC ${SOURCE_FILES})

#----------- dependencies -----------

foreach(flavour ${PROJECT_NAME} ${PROJECT_NAME}_static)
  target_link_libraries(${flavour} PUBLIC
    igraphic
    iaudio

    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
  )
  # la version statique peut finir dans une shared en aval : PIC des le depart
  set_target_properties(${flavour} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endforeach()

# plus de dependance a maths : SfmlPolygon porte son propre decoupage en
# oreilles, et Triangle / Vector2f / epsilond viennent de system.

set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

#----------- includes -----------

foreach(flavour ${PROJECT_NAME} ${PROJECT_NAME}_static)
  target_include_directories(${flavour} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/window
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/graphic
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/event
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
  )
endforeach()

# sfml_impl.dylib, pas libsfml_impl.dylib : c'est ce nom que
# ModuleManager::Load() recoit dans les tests et les exemples.
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

#----------- examples -----------

# Les deux facons de recuperer le vendor, cote a cote.

# dynamique : utilitaires + modulemanager + contrats. Ne linke PAS le vendor, il
# le dlopen au runtime - d'ou l'argv[1] qui accepte n'importe quelle dll.
add_executable(${PROJECT_NAME}_demo_dynamic examples/dynamic.cpp)
add_dependencies(${PROJECT_NAME}_demo_dynamic ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}_demo_dynamic PRIVATE examples)
target_compile_definitions(${PROJECT_NAME}_demo_dynamic PRIVATE
  SFML_IMPL_PATH="$<TARGET_FILE:${PROJECT_NAME}>"
  ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
target_link_libraries(${PROJECT_NAME}_demo_dynamic PRIVATE
  igraphic
  iaudio
  modulemanager
  ${CMAKE_DL_LIBS}
)

# statique : utilitaires + vendor + contrats. Ni modulemanager ni ${CMAKE_DL_LIBS}
# dans cette liste - c'est la difference, et elle se lit ici. Les include
# dirs viennent de la cible statique elle-meme, plus rien a recopier.
add_executable(${PROJECT_NAME}_demo_static examples/static.cpp)

target_include_directories(${PROJECT_NAME}_demo_static PRIVATE examples)
target_compile_definitions(${PROJECT_NAME}_demo_static PRIVATE
  ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
target_link_libraries(${PROJECT_NAME}_demo_static PRIVATE ${PROJECT_NAME}_static)

#----------- tests -----------

find_package(GoogleTest REQUIRED)

enable_testing()

file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)

add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})
add_dependencies(${PROJECT_NAME}_tests ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}_tests PRIVATE
  SFML_IMPL_PATH="$<TARGET_FILE:${PROJECT_NAME}>"
  ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

target_link_libraries(${PROJECT_NAME}_tests
  gtest_main
  igraphic
  iaudio
  modulemanager
  ${CMAKE_DL_LIBS}
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
