file(GLOB SRC
  App/Main.cpp
)

add_executable(eizness ${SRC})
target_link_libraries(eizness core ${SDL2_LIBRARIES})
