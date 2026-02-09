// WASM游戏桥接层 - 严格对应C++内核接口

// ============ 类型定义 ============
// Emscripten模块接口定义
interface EmscriptenModule {
  HEAPF32: {
    buffer: ArrayBuffer
  }
  _malloc(size: number): number
  _free(ptr: number): void
  _game_init(): void
  _game_start(): void // 新增
  _game_update(currentTime: number): void
  _game_jump(): number
  _game_restart(): void
  _game_get_state_array(): number
  _game_is_playing(): number
  _game_is_game_over(): number
  _game_get_score(): number
  _game_get_high_score(): number
  getValue(ptr: number, type: string): number
  setValue(ptr: number, value: number, type: string): void
}

// Emscripten模块配置选项
interface EmscriptenModuleOptions {
  locateFile?: (path: string, prefix?: string) => string
  [key: string]: unknown
}

// Emscripten模块工厂函数
type EmscriptenModuleFactory = (options?: EmscriptenModuleOptions) => Promise<EmscriptenModule>

// 扩展全局Window接口
declare global {
  interface Window {
    GameModule?: EmscriptenModuleFactory
  }
}

// ============ 游戏状态接口 ============
export interface ParsedGameState {
  dino: {
    x: number
    y: number
    width: number
    height: number
    isJumping: boolean
    isDead: boolean
  }
  groundOffset: number
  gameSpeed: number
  score: number
  highScore: number
  obstacles: Array<{
    x: number
    y: number
    width: number
    height: number
    type: 'small' | 'big'
  }>
}

// ============ 游戏桥接类 ============
class GameBridge {
  private module: EmscriptenModule | null = null
  private isInitialized = false
  private stateBufferPtr = 0
  private stateBuffer: Float32Array | null = null

  // 检查WASM模块是否可用
  private isWasmAvailable(): boolean {
    return typeof WebAssembly !== 'undefined' && WebAssembly.validate !== undefined
  }

  // 初始化WASM模块
  async init(): Promise<boolean> {
    if (this.isInitialized) return true

    if (!this.isWasmAvailable()) {
      console.error('浏览器不支持WebAssembly')
      return false
    }

    try {
      return await this.loadWasmModule()
    } catch (error) {
      console.error('WASM模块初始化错误:', error)
      return false
    }
  }

  // 加载WASM模块 - 修正 Promise 类型
  private async loadWasmModule(): Promise<boolean> {
    // 检查是否已存在 script 标签
    const existingScript = document.querySelector('script[src*="game.js"]')
    if (existingScript) {
      console.log('游戏脚本已加载')
      try {
        await this.initializeModule()
        return true
      } catch {
        return false
      }
    }

    // 创建并加载 script 标签
    return new Promise<boolean>((resolve) => {
      const script = document.createElement('script')
      //script.src = '/game.js'
      script.src = new URL('./game.js', import.meta.url).href
      script.async = true
      script.defer = true

      script.onload = async () => {
        try {
          await this.initializeModule()
          resolve(true)
        } catch (error) {
          console.error('WASM模块初始化失败:', error)
          resolve(false)
        }
      }

      script.onerror = () => {
        console.error('无法加载game.js脚本')
        resolve(false)
      }

      document.head.appendChild(script)
    })
  }

  // 初始化模块 - 移除了返回类型，改为抛出异常的方式
  private async initializeModule(): Promise<void> {
    const moduleFactory = window.GameModule

    if (!moduleFactory) {
      throw new Error('GameModule工厂函数未找到')
    }

    // 初始化WASM模块 - 修正 locateFile 签名
    this.module = await moduleFactory({
      // locateFile: (path: string) => {
      //   // 只使用第一个参数
      //   console.log('加载文件:', path)
      //   if (path.endsWith('.wasm')) {
      //     return '/game.wasm'
      //   }
      //   return '/' + path
      // },
      locateFile: (path: string) => {
        // 或更健壮：用当前模块路径
        const basePath = new URL('.', import.meta.url).href
        return basePath + path
      },
    })

    console.log('Emscripten模块加载完成:', this.module)

    // 初始化游戏
    this.module._game_init()
    console.log('游戏引擎初始化完成')

    // // 分配状态缓冲区（可选，现在先不分配，看看是否正常）
    // try {
    //   // 先不立即分配，等需要时再分配
    //   this.stateBufferPtr = 0
    //   this.stateBuffer = null
    // } catch (error) {
    //   console.warn('内存分配警告:', error)
    // }

    this.isInitialized = true
    console.log('WASM模块初始化成功')
  }

  // 开始游戏
  start(): void {
    if (!this.isInitialized || !this.module) return
    this.module._game_start()
  }

  // 更新游戏逻辑
  update(currentTime: number): void {
    if (!this.isInitialized || !this.module) return
    this.module._game_update(currentTime)
  }

  // 跳跃
  jump(): boolean {
    if (!this.isInitialized || !this.module) return false
    return this.module._game_jump() === 1
  }

  // 重新开始
  restart(): void {
    if (!this.isInitialized || !this.module) return
    this.module._game_restart()
  }

  // 获取游戏状态数组
  getStateArray(): Float32Array | null {
    if (!this.isInitialized || !this.module) return null //|| !this.stateBuffer

    const resultPtr = this.module._game_get_state_array()
    if (resultPtr === 0) return null

    // C++ 将障碍物数量作为 float 写入（作为可移植方式），因此以 'float' 读取并取整
    const obstacleCountRaw = this.module.getValue(resultPtr + 10 * 4, 'float')
    const obstacleCount = Math.floor(obstacleCountRaw)

    if (obstacleCount < 0) {
      console.warn('无效的障碍物数量（负数）:', obstacleCount)
      return null
    }

    if (obstacleCount > 100) {
      // 设置一个合理的上限
      console.warn('障碍物数量过多:', obstacleCount)
      return null
    }

    const totalLength = 11 + obstacleCount * 5
    // 添加长度检查
    if (totalLength <= 0 || totalLength > 1000) {
      console.error('无效的总长度:', totalLength)
      return null
    }

    try {
      const stateArray = new Float32Array(totalLength)

      for (let i = 0; i < totalLength; i++) {
        stateArray[i] = this.module.getValue(resultPtr + i * 4, 'float')
      }

      return stateArray
    } catch (error) {
      console.error('创建状态数组失败:', error)
      return null
    }
  }

  // 解析游戏状态
  // gameBridge.ts - 修正 parseGameState 函数
  parseGameState(): ParsedGameState | null {
    const stateArray = this.getStateArray()
    if (!stateArray) return null

    // 如果数组太短，直接返回null
    if (stateArray.length < 11) {
      console.warn('状态数组长度不足:', stateArray.length)
      return null
    }

    let index = 0

    const dino = {
      x: stateArray[index++],
      y: stateArray[index++],
      width: stateArray[index++],
      height: stateArray[index++],
      isJumping: stateArray[index++] > 0.5,
      isDead: stateArray[index++] > 0.5,
    }

    const groundOffset = stateArray[index++]
    const gameSpeed = stateArray[index++]
    const score = stateArray[index++]
    const highScore = stateArray[index++]
    let obstacleCount = Math.floor(stateArray[index++]) // 确保是整数

    // 验证障碍物数量
    const remainingItems = stateArray.length - index
    const expectedItems = obstacleCount * 5

    if (remainingItems < expectedItems) {
      console.warn('状态数组数据不完整，无法解析所有障碍物')
      // 调整障碍物数量以适应可用数据
      const actualObstacleCount = Math.max(0, Math.floor(remainingItems / 5))
      console.log(`调整障碍物数量: ${obstacleCount} -> ${actualObstacleCount}`)
      // 使用调整后的障碍物数量，避免后续读取越界
      obstacleCount = actualObstacleCount
    }

    const obstacles: ParsedGameState['obstacles'] = []
    for (let i = 0; i < obstacleCount; i++) {
      const x = stateArray[index++]
      const y = stateArray[index++]
      const width = stateArray[index++]
      const height = stateArray[index++]
      const typeValue = stateArray[index++]
      // C++ 中 small 对应 1.0f，big 对应 0.0f，因此这里映射要与之保持一致
      const type = typeValue > 0.5 ? 'small' : 'big'

      // 防护：确保数值有效，避免 NaN/Infinity 导致渲染异常
      const ox = Number.isFinite(x) ? x : 0
      const oy = Number.isFinite(y) ? y : 0
      const ow = Number.isFinite(width) && width > 0 ? width : 1
      const oh = Number.isFinite(height) && height > 0 ? height : 1

      obstacles.push({
        x: ox,
        y: oy,
        width: ow,
        height: oh,
        type,
      })
    }

    return {
      dino,
      groundOffset,
      gameSpeed,
      score,
      highScore,
      obstacles,
    }
  }

  // 是否正在游戏中
  isPlaying(): boolean {
    if (!this.isInitialized || !this.module) return false
    return this.module._game_is_playing() === 1
  }

  // 是否游戏结束
  isGameOver(): boolean {
    if (!this.isInitialized || !this.module) return false
    return this.module._game_is_game_over() === 1
  }

  // 获取当前分数
  getScore(): number {
    if (!this.isInitialized || !this.module) return 0
    return this.module._game_get_score()
  }

  // 获取最高分
  getHighScore(): number {
    if (!this.isInitialized || !this.module) return 0
    return this.module._game_get_high_score()
  }

  // 清理资源
  cleanup(): void {
    if (this.stateBufferPtr && this.module) {
      this.module._free(this.stateBufferPtr)
    }
    this.stateBuffer = null
    this.stateBufferPtr = 0
    this.isInitialized = false
    this.module = null
  }
}

// 导出单例实例
export const gameBridge = new GameBridge()
