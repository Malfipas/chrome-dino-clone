import { fileURLToPath, URL } from 'node:url'
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'

// https://vite.dev/config/
export default defineConfig({
  base: '/chrome-dino-clone/',
  plugins: [vue(), vueDevTools()],
  optimizeDeps: {
    exclude: ['game.js'], // 排除WASM文件从依赖优化
  },
  assetsInclude: ['**/*.wasm'], // 注意：assetsInclude 应该在根级别，不在 build 内部
  server: {
    headers: {
      'Cross-Origin-Opener-Policy': 'same-origin',
      'Cross-Origin-Embedder-Policy': 'require-corp',
    },
  },
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url)),
    },
  },
  build: {
    // 如果需要额外的构建配置可以放在这里
    target: 'esnext',
  },
})
