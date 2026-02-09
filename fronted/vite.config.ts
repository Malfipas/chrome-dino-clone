import { fileURLToPath, URL } from 'node:url'
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'

// https://vite.dev/config/
export default defineConfig({
  //base: '/chrome-dino-clone/', //生产环境-即部署到GitHub Pages时的路径
  base: '/', //开发环境-即本地运行时的路径
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
    outDir: 'dist',
    assetsDir: 'assets',
    // 确保 game.js, game.wasm, sprite.png 不被重命名
    rollupOptions: {
      output: {
        assetFileNames: (assetInfo) => {
          const name = assetInfo.name || ''
          if (['game.js', 'game.wasm', 'sprite.png'].includes(name)) {
            return '[name].[ext]' // 保持原名
          }
          return 'assets/[name]-[hash].[ext]'
        },
      },
    },
  },
})
