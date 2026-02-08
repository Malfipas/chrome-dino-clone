import { defineStore } from 'pinia'

interface EngineState {
  score?: {
    score: number
    highScore: number
  }
  gameSpeed?: number
  gameState?: 'IDLE' | 'PLAYING' | 'GAME_OVER'
}

interface GameStoreState {
  gameState: 'IDLE' | 'PLAYING' | 'GAME_OVER'
  score: number
  highScore: number
  newRecord: boolean
  gameSpeed: number
}

export const useGameStore = defineStore('game', {
  state: (): GameStoreState => ({
    gameState: 'IDLE',
    score: 0,
    highScore: 0,
    newRecord: false,
    gameSpeed: 0,
  }),

  actions: {
    startGame() {
      this.gameState = 'PLAYING'
      this.score = 0
      this.newRecord = false
    },

    endGame() {
      this.gameState = 'GAME_OVER'
    },

    resetGame() {
      this.gameState = 'IDLE'
      this.score = 0
    },

    setHighScore(score: number) {
      this.highScore = score
    },

    updateFromEngine(engineState: EngineState) {
      if (engineState) {
        this.score = engineState.score?.score || 0
        this.highScore = engineState.score?.highScore || 0
        this.gameSpeed = engineState.gameSpeed || 0
        this.gameState = engineState.gameState || 'IDLE'
      }
    },
  },
})
