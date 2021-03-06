include_directories(
./include/
../engine/include/
)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB components
    "source/components/*.cpp"
)

file(GLOB systems
    "source/systems/*.cpp"
)

add_executable(r-type_server
                            source/main.cpp
                            source/LevelState.cpp
                            source/LobbyState.cpp
                            source/CreditsState.cpp
                            source/GameOverState.cpp
                            source/GameServer.cpp
                            source/AssetsServer.cpp
                            source/Storage.cpp
                            source/Enemies.cpp
                            source/load_game.cpp
                            ${components}
                            ${systems}
)

target_link_libraries(r-type_server r-type_engine ${CMAKE_DL_LIBS})

add_executable(test_oriented_sprite
  tests/src/test_oriented_sprite.cpp
  source/components/OrientedSprite.cpp)
