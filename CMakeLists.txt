cmake_minimum_required(VERSION 3.26)
project(Minigin)

if(EMSCRIPTEN)
  message("Using Emscripten!")
endif()


if(EMSCRIPTEN)
  set(GAME_TARGET   game_web)
else()
  set(GAME_TARGET   game)
endif()

set(ENGINE_TARGET minigin)


file(GLOB_RECURSE MINIGIN_SOURCES "Minigin/*.cpp" "Minigin/*.h")
file(GLOB_RECURSE GAME_SOURCES    "Game/*.cpp"    "Game/*.h")


add_library(${ENGINE_TARGET} STATIC ${MINIGIN_SOURCES})
target_compile_features(${ENGINE_TARGET} PUBLIC cxx_std_20)


if(EMSCRIPTEN)
  add_executable(${GAME_TARGET} ${GAME_SOURCES})
else()
  add_executable(${GAME_TARGET} WIN32 ${GAME_SOURCES})
endif()
target_compile_features(${GAME_TARGET} PUBLIC cxx_std_20)


include(FetchContent)

# glm
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
  FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_MakeAvailable(glm)
endif()

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_NO_PROGRESS ON
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
)
FetchContent_MakeAvailable(nlohmann_json)

# SDL3
find_package(SDL3 3.4 CONFIG QUIET)
if(NOT SDL3_FOUND)
  if(WIN32)
    FetchContent_Declare(
      SDL3
      URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}")
    find_package(SDL3 CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG        release-3.4.0
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(SDL3)
  endif()
endif()

# SDL3_ttf
find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
  if(WIN32)
    FetchContent_Declare(
      SDL3_ttf
      URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
    find_package(SDL3_ttf CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG        release-3.2.2
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
    )
    set(SDL3TTF_INSTALL OFF)
    if(EMSCRIPTEN)
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten")
      set(SDL3TTF_USE_SYSTEM_FREETYPE ON)
      set(SDL3TTF_VENDORED           OFF)
      set(SDLTTF_PLUTOSVG            OFF)
      add_compile_options(-sUSE_SDL=2 -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
      add_link_options(  -sUSE_SDL=2 -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
    endif()
    FetchContent_MakeAvailable(SDL3_ttf)
  endif()
endif()

# SDL3_mixer
find_package(SDL3_mixer CONFIG QUIET)
if(NOT SDL3_mixer_FOUND)
  if(WIN32)
    FetchContent_Declare(
      SDL3_mixer
      URL https://www.libsdl.org/projects/SDL_mixer/release/SDL3_mixer-devel-3.2.0-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_mixer)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_mixer_SOURCE_DIR}")
    find_package(SDL3_mixer CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3_mixer
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
      GIT_TAG        release-3.2.0
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
    )
    set(SDL3MIXER_INSTALL OFF)
    FetchContent_MakeAvailable(SDL3_mixer)
  endif()
endif()

# Dear ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG        v1.92.5-docking
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${sdl3_SOURCE_DIR}/include)
target_link_libraries(imgui PUBLIC SDL3::SDL3)
target_compile_features(imgui PUBLIC cxx_std_20)

# VLD (Windows only)
if(WIN32)
  find_package(VLD CONFIG)
endif()

# Steam
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/steam")
find_package(Steamworks QUIET)

target_include_directories(${ENGINE_TARGET} PUBLIC
  ${CMAKE_SOURCE_DIR}/Minigin/
  ${sdl3_SOURCE_DIR}/include
  ${sdl3_ttf_SOURCE_DIR}/include
  ${sdl3_mixer_SOURCE_DIR}/include
  ${imgui_SOURCE_DIR}
  ${VLD_INCLUDE_DIR}
)

target_link_libraries(${ENGINE_TARGET} PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  SDL3_mixer::SDL3_mixer
  glm::glm
  imgui
  nlohmann_json::nlohmann_json
  ${VLD_LIBRARIES}
)

if(WIN32)
  target_link_libraries(${ENGINE_TARGET} PUBLIC xinput)
endif()


target_include_directories(${GAME_TARGET} PRIVATE
  ${CMAKE_SOURCE_DIR}/Game/
)

target_link_libraries(${GAME_TARGET} PRIVATE ${ENGINE_TARGET})


foreach(_tgt ${ENGINE_TARGET} ${GAME_TARGET})
  if(MSVC)
    target_compile_options(${_tgt} PRIVATE /W4 /WX /EHsc)
  else()
    target_compile_options(${_tgt} PRIVATE -Wall -Wextra -Werror)
  endif()
endforeach()


if(EMSCRIPTEN)
  set(EMSCRIPTEN_COMPILE_FLAGS "-g -sUSE_SDL=2 -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1")
  set(EMSCRIPTEN_LINK_FLAGS
    "-sUSE_SDL=2 -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH --preload-file \"${CMAKE_SOURCE_DIR}/Data@/\""
  )

  set_target_properties(${GAME_TARGET} PROPERTIES
    COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
    LINK_FLAGS    "${EMSCRIPTEN_LINK_FLAGS}"
  )

  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/web"
      "$<TARGET_FILE_DIR:${GAME_TARGET}>/"
  )

  set(CMAKE_EXECUTABLE_SUFFIX ".html")

elseif(WIN32)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${GAME_TARGET})
  set_target_properties(${GAME_TARGET} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${GAME_TARGET}>"
  )

  foreach(dllTarget SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_mixer::SDL3_mixer)
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${dllTarget}>"
        "$<TARGET_FILE_DIR:${GAME_TARGET}>"
      VERBATIM
    )
  endforeach()

  if(Steamworks_FOUND)
    target_link_libraries(${ENGINE_TARGET} PUBLIC Steamworks::Steamworks)
    target_compile_definitions(${ENGINE_TARGET} PRIVATE USE_STEAMWORKS)
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy "${Steamworks_RUNTIME_DLL}" "$<TARGET_FILE_DIR:${GAME_TARGET}>")
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${GAME_TARGET}>/steam_appid.txt")
  endif()

  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Data"
      "$<TARGET_FILE_DIR:${GAME_TARGET}>/Data"
  )

elseif(APPLE)
  set_target_properties(${GAME_TARGET} PROPERTIES
    MACOSX_BUNDLE      TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
  )
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory     "$<TARGET_BUNDLE_DIR:${GAME_TARGET}>/Contents/Frameworks"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different  "$<TARGET_FILE:SDL3::SDL3-shared>"              "$<TARGET_BUNDLE_DIR:${GAME_TARGET}>/Contents/Frameworks/"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different  "$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>"      "$<TARGET_BUNDLE_DIR:${GAME_TARGET}>/Contents/Frameworks/"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different  "$<TARGET_FILE:SDL3_mixer::SDL3_mixer-shared>"  "$<TARGET_BUNDLE_DIR:${GAME_TARGET}>/Contents/Frameworks/"
    COMMAND install_name_tool -change "@rpath/libSDL3_ttf.0.dylib"   "@executable_path/../Frameworks/libSDL3_ttf.0.2.2.dylib"  "$<TARGET_FILE:${GAME_TARGET}>"
    COMMAND install_name_tool -change "@rpath/libSDL3_mixer.0.dylib" "@executable_path/../Frameworks/libSDL3_mixer.0.dylib"    "$<TARGET_FILE:${GAME_TARGET}>"
    COMMAND install_name_tool -change "@rpath/libSDL3.0.dylib"       "@executable_path/../Frameworks/libSDL3.0.dylib"          "$<TARGET_FILE:${GAME_TARGET}>"
  )

  if(Steamworks_FOUND)
    target_link_libraries(${ENGINE_TARGET} PUBLIC Steamworks::Steamworks)
    target_compile_definitions(${ENGINE_TARGET} PRIVATE USE_STEAMWORKS)
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${Steamworks_LIBRARIES}"
        "$<TARGET_BUNDLE_DIR:${GAME_TARGET}>/Contents/Frameworks/"
    )
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${GAME_TARGET}>/steam_appid.txt")
  endif()

  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Data"
      "$<TARGET_FILE_DIR:${GAME_TARGET}>/Data"
  )

else()
  if(Steamworks_FOUND)
    target_link_libraries(${ENGINE_TARGET} PUBLIC Steamworks::Steamworks)
    target_compile_definitions(${ENGINE_TARGET} PRIVATE USE_STEAMWORKS)
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${Steamworks_LIBRARIES}"
        "$<TARGET_FILE_DIR:${GAME_TARGET}>"
    )
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${GAME_TARGET}>/steam_appid.txt")
  endif()

  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Data"
      "$<TARGET_FILE_DIR:${GAME_TARGET}>/Data"
  )
endif()