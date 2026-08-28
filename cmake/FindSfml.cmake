set(name sfml)
set(SFML_VERSION "3.0.2")

include(FetchContent)
# Check if it's already fetched
if (NOT ${name}_FOUND) # If there's none, fetch and build raylib
  # If not, fetch it
  FetchContent_Declare(
    ${name}
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG        "${SFML_VERSION}"
  )

  set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libs" FORCE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Enable PIC for static libs" FORCE)

  FetchContent_MakeAvailable(${name})
endif()