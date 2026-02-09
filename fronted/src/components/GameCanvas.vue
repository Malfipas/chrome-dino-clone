<template>
  <div class="game-wrapper">
    <canvas
      ref="gameCanvas"
      :width="canvasWidth"
      :height="canvasHeight"
      tabindex="0"
      class="game-canvas"
    ></canvas>

    <!-- 游戏状态覆盖层 -->
    <div v-if="!isPlaying" class="game-overlay">
      <div v-if="gameState === 'IDLE'" class="start-screen">
        <h2>Chrome Dino Clone (C++ Core)</h2>
        <p>点击屏幕然后按空格键开始游戏</p>
      </div>

      <div v-if="gameState === 'GAME_OVER'" class="game-over-screen">
        <h2>游戏结束</h2>
        <p>得分: {{ score }}</p>
        <p v-if="newRecord">🎉 新纪录! 🎉</p>
        <p>最高分: {{ highScore }}</p>
        <button @click="restartGame">重新开始</button>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, computed } from 'vue'
import { useGameStore } from '../stores/gameStore'
import { gameBridge } from '../wasm/gameBridge'
import { CANVAS_WIDTH, CANVAS_HEIGHT, GROUND_Y, DINO, OBSTACLES } from '../core/constants'

interface DinoState {
  x: number
  y: number
  width: number
  height: number
  isJumping: boolean
  isDead: boolean
  sprite?: {
    x: number
    y: number
    w: number
    h: number
  }
}

interface ObstacleState {
  x: number
  y: number
  width: number
  height: number
  type: 'small' | 'big'
}

interface GameEngineState {
  dino: DinoState
  groundOffset: number
  gameSpeed: number
  score: number
  highScore: number
  obstacles: ObstacleState[]
}

const gameStore = useGameStore()
const gameCanvas = ref<HTMLCanvasElement | null>(null)
const ctx = ref<CanvasRenderingContext2D | null>(null)

const canvasWidth = CANVAS_WIDTH
const canvasHeight = CANVAS_HEIGHT

const spriteImage = new Image()
let lastRenderTime = 0
let animationFrameId = 0
let wasmInitialized = false

const gameState = computed(() => gameStore.gameState)
const isPlaying = computed(() => gameState.value === 'PLAYING')
const score = computed(() => gameStore.score)
const highScore = computed(() => gameStore.highScore)
const newRecord = computed(() => gameStore.newRecord)

onMounted(async () => {
  await initGame()

  // 监听全局键盘事件
  window.addEventListener('keydown', handleGlobalKeyDown)
  window.addEventListener('keyup', handleGlobalKeyUp)
})

onUnmounted(() => {
  if (animationFrameId) {
    cancelAnimationFrame(animationFrameId)
  }
  window.removeEventListener('keydown', handleGlobalKeyDown)
  window.removeEventListener('keyup', handleGlobalKeyUp)
  gameBridge.cleanup()
})

const initGame = async () => {
  if (!gameCanvas.value) return

  ctx.value = gameCanvas.value.getContext('2d')

  // 加载精灵图
  spriteImage.onload = () => {
    console.log('精灵图加载完成')

    // 初始化WASM模块
    initWasm()
  }

  spriteImage.onerror = () => {
    console.error('精灵图加载失败，继续初始化WASM（使用占位渲染）')
    // 即便精灵加载失败，也应初始化WASM以允许游戏逻辑运行并避免无法启动的问题
    initWasm()
    gameLoop()
  }
  // 精灵图路径
  spriteImage.src = './sprite.png'
  //spriteImage.src = new URL('./sprite.png', import.meta.url).href
}

const initWasm = async () => {
  try {
    wasmInitialized = await gameBridge.init()
    if (wasmInitialized) {
      console.log('WASM游戏引擎初始化成功')
      gameLoop()
    } else {
      console.error('WASM游戏引擎初始化失败')
      gameLoop()
    }
  } catch (error) {
    console.error('WASM初始化错误:', error)
    gameLoop()
  }
}

const gameLoop = (currentTime = 0) => {
  if (lastRenderTime === 0) {
    lastRenderTime = currentTime
  }

  //const deltaTime = currentTime - lastRenderTime
  lastRenderTime = currentTime

  // 更新游戏逻辑
  if (wasmInitialized) {
    // 检查游戏状态
    const isPlaying = gameBridge.isPlaying()
    const isGameOver = gameBridge.isGameOver()

    // 同步状态到store
    if (isGameOver && gameState.value !== 'GAME_OVER') {
      gameStore.endGame()
    } else if (isPlaying && gameState.value !== 'PLAYING') {
      gameStore.startGame()
    } else if (!isPlaying && !isGameOver && gameState.value === 'PLAYING') {
      // 如果WASM说不是PLAYING，但前端是PLAYING，重置为IDLE
      gameStore.resetGame()
    }

    // 只有在游戏进行中时才更新
    if (gameBridge.isPlaying()) {
      gameBridge.update(currentTime)
    }

    // 从WASM获取游戏状态
    const engineState = gameBridge.parseGameState()

    if (engineState) {
      // 更新存储状态
      gameStore.updateFromEngine({
        score: {
          score: Math.floor(engineState.score),
          highScore: Math.floor(engineState.highScore),
        },
        gameSpeed: engineState.gameSpeed,
        gameState: gameBridge.isGameOver()
          ? 'GAME_OVER'
          : gameBridge.isPlaying()
            ? 'PLAYING'
            : 'IDLE',
      })

      // 渲染游戏
      renderGame(engineState)
    } else {
      // 如果获取状态失败，显示占位符
      if (ctx.value) {
        ctx.value.fillStyle = '#ffffff'
        ctx.value.fillRect(0, 0, canvasWidth, canvasHeight)
        ctx.value.fillStyle = '#000000'
        ctx.value.font = '20px Arial'
        ctx.value.fillText('等待游戏状态...', 20, 50)
      }
    }
  }

  animationFrameId = requestAnimationFrame(gameLoop)
}

const renderGame = (engineState: GameEngineState) => {
  if (!ctx.value || !spriteImage.complete) return

  // 清空画布
  ctx.value.fillStyle = '#ffffff'
  ctx.value.fillRect(0, 0, canvasWidth, canvasHeight)

  // 绘制地面
  drawGround(engineState.groundOffset)

  // 绘制障碍物
  engineState.obstacles.forEach((obstacle) => {
    drawObstacle(obstacle)
  })

  // 绘制恐龙
  drawDino(engineState.dino)

  // 绘制分数
  drawScore()
}

const drawGround = (offset: number) => {
  const groundY = GROUND_Y
  const groundHeight = 18

  if (!ctx.value) return

  if (spriteImage.complete) {
    ctx.value.drawImage(
      spriteImage,
      0,
      104,
      2404,
      groundHeight,
      -offset,
      groundY,
      2404,
      groundHeight,
    )

    ctx.value.drawImage(
      spriteImage,
      0,
      104,
      2404,
      groundHeight,
      2404 - offset,
      groundY,
      2404,
      groundHeight,
    )
  }

  ctx.value.fillStyle = '#000000'
  ctx.value.fillRect(0, groundY + groundHeight, canvasWidth, 2)
}

const drawDino = (dino: DinoState) => {
  if (!spriteImage.complete || !ctx.value) return

  // 根据恐龙状态选择精灵图
  let sprite
  if (dino.isDead) {
    sprite = DINO.SPRITES.DEAD
  } else if (dino.isJumping) {
    sprite = DINO.SPRITES.JUMP
  } else {
    // 跑步动画
    const frame = Math.floor(Date.now() / 100) % 2
    sprite = frame === 0 ? DINO.SPRITES.RUN_1 : DINO.SPRITES.RUN_2
  }

  ctx.value.drawImage(
    spriteImage,
    sprite.x,
    sprite.y,
    sprite.w,
    sprite.h,
    dino.x,
    dino.y,
    dino.width,
    dino.height,
  )
}

const drawObstacle = (obstacle: ObstacleState) => {
  if (!spriteImage.complete || !ctx.value) return

  const config = obstacle.type === 'small' ? OBSTACLES.SMALL : OBSTACLES.BIG

  // 使用精灵图中的位置
  const spriteX = config.SPRITE_X
  const spriteY = config.SPRITE_Y

  ctx.value.drawImage(
    spriteImage,
    spriteX,
    spriteY,
    config.WIDTH,
    config.HEIGHT,
    obstacle.x,
    obstacle.y,
    obstacle.width,
    obstacle.height,
  )
  // ctx.value.drawImage(
  //   spriteImage,
  //   config.SPRITE_X,
  //   config.SPRITE_Y,
  //   config.WIDTH,
  //   config.HEIGHT,
  //   obstacle.x,
  //   obstacle.y,
  //   obstacle.width,
  //   obstacle.height,
  //)
}

const drawScore = () => {
  if (!ctx.value) return

  ctx.value.font = '20px Arial'
  ctx.value.fillStyle = '#000000'
  ctx.value.textAlign = 'left'

  ctx.value.fillText(`分数: ${gameStore.score}`, 20, 30)
  ctx.value.fillText(`最高: ${gameStore.highScore}`, 20, 60)
}

const handleGlobalKeyDown = (event: KeyboardEvent) => {
  // 忽略在输入框或可编辑元素中的按键
  const target = event.target as HTMLElement | null
  if (
    target &&
    (target.tagName === 'INPUT' || target.tagName === 'TEXTAREA' || target.isContentEditable)
  )
    return

  if (['Space', 'ArrowDown'].includes(event.code)) {
    event.preventDefault()
    handleKeyAction(event.code, true)
  }
}

const handleGlobalKeyUp = (event: KeyboardEvent) => {
  const target = event.target as HTMLElement | null
  if (
    target &&
    (target.tagName === 'INPUT' || target.tagName === 'TEXTAREA' || target.isContentEditable)
  )
    return

  if (['Space', 'ArrowDown'].includes(event.code)) {
    event.preventDefault()
    handleKeyAction(event.code, false)
  }
}

const handleKeyAction = (keyCode: string, isKeyDown: boolean) => {
  console.log(`按键: ${keyCode}, 状态: ${isKeyDown ? '按下' : '释放'}`)

  switch (keyCode) {
    case 'Space':
      if (isKeyDown) {
        if (gameState.value === 'IDLE') {
          // console.log('开始新游戏')
          // if (wasmInitialized) {
          //   // 调用WASM的start方法开始游戏
          //   gameBridge.start()
          //   gameStore.startGame()
          //   // 同时让恐龙跳跃
          //   gameBridge.jump()
          // }
          console.log('开始新游戏')
          if (wasmInitialized) {
            // 先重启游戏确保状态正确
            gameBridge.restart()
            // 等待一下再开始
            setTimeout(() => {
              gameBridge.start()
              gameStore.startGame()
              // 开始后立即跳跃
              setTimeout(() => {
                gameBridge.jump()
              }, 50)
            }, 100)
          }
        } else if (gameState.value === 'GAME_OVER') {
          console.log('重新开始游戏')
          if (wasmInitialized) {
            gameBridge.restart()
            gameStore.resetGame()
            // 重启后需要手动开始游戏
            setTimeout(() => {
              gameBridge.start()
              gameStore.startGame()
            }, 100)
          }
        } else if (gameState.value === 'PLAYING') {
          if (wasmInitialized) {
            const jumped = gameBridge.jump()
            console.log('跳跃:', jumped ? '成功' : '失败')
          }
        }
      }
      break
  }
}

const restartGame = () => {
  if (wasmInitialized) {
    gameBridge.restart()
  }
  gameStore.resetGame()
}
</script>

<style scoped>
/* 样式保持不变 */
.game-wrapper {
  position: relative;
  display: inline-block;
}

.game-canvas {
  border: 2px solid #333;
  display: block;
  background-color: #f7f7f7;
  outline: none;
}

.game-canvas:focus {
  border-color: #4caf50;
  box-shadow: 0 0 5px #4caf50;
}

.game-overlay {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(255, 255, 255, 0.85);
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  font-family: 'Courier New', monospace;
  text-align: center;
}

.start-screen {
  max-width: 80%;
  padding: 40px;
  background-color: rgba(255, 255, 255, 0.95);
  border-radius: 15px;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
  display: flex;
  flex-direction: column;
  align-items: center;
}

.start-screen h2 {
  font-size: 36px;
  margin-bottom: 20px;
  color: #333;
}

.start-screen p {
  font-size: 18px;
  color: #666;
  animation: blink 1.5s infinite;
}

.game-over-screen {
  max-width: 80%;
  padding: 40px;
  background-color: rgba(255, 255, 255, 0.95);
  border-radius: 15px;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
  display: flex;
  flex-direction: column;
  align-items: center;
}

.game-over-screen h2 {
  font-size: 32px;
  color: #e74c3c;
  margin-bottom: 15px;
}

.game-over-screen p {
  font-size: 18px;
  margin: 5px 0;
  color: #333;
}

.game-over-screen button {
  margin-top: 20px;
  padding: 10px 20px;
  font-size: 16px;
  background-color: #4caf50;
  color: white;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  transition: background-color 0.3s;
}

.game-over-screen button:hover {
  background-color: #45a049;
}

@keyframes blink {
  0%,
  100% {
    opacity: 1;
  }
  50% {
    opacity: 0.5;
  }
}
</style>
