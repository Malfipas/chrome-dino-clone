// 游戏物理参数
export const GRAVITY = 1.5 // 重力（与C++同步）
export const JUMP_FORCE = -38 // 跳跃力度（与C++同步）
export const INITIAL_GAME_SPEED = 13 // 初始速度（与C++同步）
export const MAX_GAME_SPEED = 30

// 游戏对象尺寸（基于精灵图）
export const DINO = {
  WIDTH: 89,
  HEIGHT: 94,
  SPRITES: {
    RUN_1: { x: 1514, y: 0, w: 88, h: 94 },
    RUN_2: { x: 1602, y: 0, w: 88, h: 94 },
    JUMP: { x: 1338, y: 0, w: 88, h: 94 },
    DEAD: { x: 1788, y: 0, w: 88, h: 94 }, // 游戏结束特化模型
  },
}

export const OBSTACLES = {
  SMALL: {
    WIDTH: 34,
    HEIGHT: 70,
    SPRITE_X: 446,
    SPRITE_Y: 2,
  },
  BIG: {
    WIDTH: 49,
    HEIGHT: 100,
    SPRITE_X: 652,
    SPRITE_Y: 2,
  },
}

// 游戏区域
export const CANVAS_WIDTH = 1300
export const CANVAS_HEIGHT = 800
export const GROUND_Y = 600
export const DINO_X = 100

// 游戏逻辑
export const SCORE_INCREMENT_INTERVAL = 5 // 每5帧增加1分
export const OBSTACLE_SPAWN_RANGE = { min: 1800, max: 2500 } // 障碍物生成间隔（增大最小间隔）
export const GAME_SPEED_INCREASE_RATE = 0.005 // 每400分增加2速度
