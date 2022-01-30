set(BUILD_SHARED_LIBS OFF)

add_executable(lsystem
  main.cpp
)

# Yep, that's it!
target_link_libraries(lsystem
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

#include(Install.cmake)
