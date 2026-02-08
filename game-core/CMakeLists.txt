cmake_minimum_required(VERSION 3.10)
project(DinoGameCore)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 源文件列表 - 只包含必要的文件
set(GAME_SOURCES
    src/CollisionSystem.cpp
    src/Dino.cpp
    src/GameBridge.cpp      # 使用这个，不是bridge.cpp
    src/GameEngine.cpp
    src/GameState.cpp
    src/ObstacleManager.cpp
    src/ScoreManager.cpp
    src/constants.cpp
)

# 检查文件是否存在
foreach(source ${GAME_SOURCES})
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
        message(WARNING "源文件不存在: ${source}")
    endif()
endforeach()

# 创建可执行的WASM模块
add_executable(game ${GAME_SOURCES})

# 设置目标属性
set_target_properties(game PROPERTIES
    OUTPUT_NAME "game"
    SUFFIX ".js"
)

# 设置Emscripten链接器标志 - 使用 target_link_options
target_link_options(game PRIVATE
    "SHELL:-s WASM=1"
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s EXPORT_NAME='GameModule'"
    "SHELL:-s EXPORTED_FUNCTIONS=['_game_init','_game_start','_game_update','_game_jump','_game_restart','_game_get_state_array','_game_is_playing','_game_is_game_over','_game_get_score','_game_get_high_score','_malloc','_free']"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','getValue','setValue','UTF8ToString','lengthBytesUTF8','stringToUTF8','HEAPF32','HEAPU8']"  # 添加 HEAPF32 和 HEAPU8
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s NO_EXIT_RUNTIME=1"
    "SHELL:-s ASSERTIONS=1"
    "SHELL:-s ENVIRONMENT=web"
    "SHELL:-O2"
)

# 设置编译器标志
target_compile_options(game PRIVATE
    -fno-exceptions
    -fno-rtti
)