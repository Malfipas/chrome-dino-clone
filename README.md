# dino-cpp-game

中英双语说明 / Bilingual README

简短说明（中文）

- 这是一个基于 C++（编译为 WebAssembly）与 Vue 3 + Vite 的简易跳跃小游戏（类似 Chrome 离线小恐龙）。
- 游戏核心位于 `game-core`（C++），前端位于 `fronted`（Vue + TypeScript）。
- 这个游戏只是一个在AI辅助下做出来的很粗糙很粗糙的成果，等我慢慢汲取编程祖师爷之力变成大佬之后也许会把它改得优雅一点……

Short description (English)

- A small Dino-style game: C++ game core compiled to WebAssembly, with a Vue 3 + Vite frontend.
- Core code: `game-core`. Frontend: `fronted`.
- This game is merely a very crude, very rough result made with the assistance of AI. After I slowly absorb the power of the programming ancestors and evolve into a master, perhaps I'll refine it to be more graceful someday...

主要功能 / Features

- C++ 写的游戏引擎，编译为 WebAssembly 加载到浏览器。
- 前端使用 Vue 3 渲染画面并与 WASM 交互。
- 可调节的物理参数（重力、跳跃力、速度、障碍生成规则）。

en ver:

- Game engine written in C++, compiled to WebAssembly and loaded into the browser.
- Frontend uses Vue 3 for rendering and interacting with WASM.
- Adjustable physics parameters (gravity, jump force, speed, obstacle generation rules).

先决条件 / Prerequisites

- Node.js 和 npm（用于前端依赖与 Vite）
- Emscripten / CMake / Ninja 或等效工具链（用于将 `game-core` 编译为 WASM）

en ver:

- Node.js and npm (for frontend dependencies and Vite)
- Emscripten / CMake / Ninja or equivalent toolchain (for compiling game-core to WASM)

快速开始 / Quick Start

<!-- markdownlint-disable MD029 MD030 -->

1. 安装前端依赖 / install frontend deps:

```bash
cd fronted
npm install
```

2. 启动开发服务器（热重载）/ start dev server:

```bash
cd fronted
npm run dev
# then open http://localhost:5173/ (or the port Vite chooses)
```

3. 构建并生成前端静态文件 / build frontend for production:

```bash
cd fronted
npm run build
```

<!-- markdownlint-enable MD029 MD030 -->

构建并部署 C++ -> WASM（开发者）/ Build game core (C++ -> WASM)

项目已经配好 CMake 构建目录 `game-core/build`。常用流程：

```bash
cd game-core
mkdir -p build
cd build
# 使用 Emscripten 的 cmake 生成器，例如 emcmake cmake ..
emcmake cmake ..
cmake --build . --config Release
# 构建完成后，输出通常为 game.js 和 game.wasm 在 game-core/build
# 将它们复制到前端 public 目录用于调试/本地运行：
Copy-Item -Path "game-core/build/game.js" -Destination "fronted/public/game.js" -Force  # PowerShell
Copy-Item -Path "game-core/build/game.wasm" -Destination "fronted/public/game.wasm" -Force
```

注意：上述复制命令为 PowerShell 示例，Linux/macOS 下使用 `cp`。

en ver:

The project is pre-configured with the CMake build directory game-core/build. Common workflow:

```bash
cd game-core
mkdir -p build
cd build
# Use Emscripten's cmake generator, e.g., emcmake cmake ..
emcmake cmake ..
cmake --build . --config Release
# After the build completes, the output is typically game.js and game.wasm in game-core/build
# Copy them to the frontend public directory for debugging/local execution:
Copy-Item -Path "game-core/build/game.js" -Destination "fronted/public/game.js" -Force  # PowerShell
Copy-Item -Path "game-core/build/game.wasm" -Destination "fronted/public/game.wasm" -Force
```

Note: The above copy command is a PowerShell example; use cp on Linux/macOS.

配置与可调参数 / Configuration & Tuning

- C++ 常量文件：`game-core/include/constants.hpp`（重力 `GRAVITY`、跳跃力 `JUMP_FORCE`、初始速度等）。
- 障碍生成逻辑：`game-core/src/ObstacleManager.cpp`（包含速度相关的生成间隔与抖动）。
- 前端镜像常量：`fronted/src/core/constants.ts`（与 C++ 常量同步以便前端表现一致）。

en ver:

- C++ constants file: game-core/include/constants.hpp (gravity GRAVITY, jump force JUMP_FORCE, initial speed, etc.).
- Obstacle generation logic: game-core/src/ObstacleManager.cpp (includes speed-related generation intervals and jitter).
- Frontend mirrored constants: fronted/src/core/constants.ts (synchronized with C++ constants for consistent frontend behavior).

开发提示 / Development notes

- 修改 C++ 源后请重新构建 WASM 并把 `game.js`/`game.wasm` 复制到 `fronted/public`，然后刷新浏览器页面。
- 前端 dev server 会自动加载 public 下的新文件，但某些缓存或浏览器行为可能需要手动刷新或清除缓存。
- 如果遇到编译错误，优先查看 `game-core/build/CMakeFiles/CMakeError.log` 与编译器输出。

en ver:

- After modifying C++ source code, please rebuild WASM, copy game.js/game.wasm to fronted/public, and refresh the browser page.
- The frontend dev server will automatically load new files from the public directory, but some caching or browser behavior may require a manual refresh or cache clear.
- If compilation errors occur, first check game-core/build/CMakeFiles/CMakeError.log and the compiler output.

常见问题 / Troubleshooting

- 浏览器没有加载 WASM：确保 `game.wasm` 与 `game.js` 在 `fronted/public`，并检查浏览器 Console/Network 是否返回 200。
- 端口冲突：Vite 默认 5173，若端口被占用会尝试下一个可用端口，终端会显示实际地址。

en ver:

- Browser not loading WASM: Ensure game.wasm and game.js are in fronted/public, and check the browser Console/Network for 200 status.
- Port conflict: Vite defaults to port 5173. If the port is occupied, it will try the next available port; the terminal will display the actual address.

文件索引 / Important files

- `game-core/` — C++ 游戏核心，包含 `CMakeLists.txt` 与源码。
- `fronted/` — Vue 前端。
- `fronted/public/` — 静态资源目录（放置 `game.js` / `game.wasm` 以便本地运行）。
- `fronted/src/core/constants.ts` — 前端常量（与 C++ 同步）。

en ver:

- `game-core/` — C++ game core, includes `CMakeLists.txt` and source code.
- `fronted/` — Vue frontend.
- `fronted/public/` — Static resource directory (place `game.js` / `game.wasm` here for local execution).
- `fronted/src/core/constants.ts` — Frontend constants (synchronized with C++).

反馈 / Contact

- 有问题或想要调整游戏手感（跳跃、速度、生成逻辑），可以直接修改 `game-core/include/constants.hpp` 并重建，或者在前端调整显示相关逻辑。

en ver:

- If you have issues or want to adjust the game feel (jumping, speed, spawning logic), you can directly modify game-core/include/constants.hpp and rebuild, or adjust display-related logic on the front-end.

---

感谢使用 / Thanks for trying out the project!
