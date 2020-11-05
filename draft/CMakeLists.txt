include_directories(
./include/
../engine/include/
)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(r-type_local
                            source/main.cpp
)

target_link_libraries(r-type_local r-type_engine)