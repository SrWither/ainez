file(GLOB SRC
  Core/NesCpu.cpp
  Core/OpCodes.cpp
)

add_library(core STATIC ${SRC})
