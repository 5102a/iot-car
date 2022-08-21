const { resolve } = require('path')
const webpack = require('webpack')
const { CleanWebpackPlugin } = require('clean-webpack-plugin')

module.exports = {
  entry: {
    library: ['vue', 'echarts', 'element-ui'], // 多个库打包，数组中可以填写多个模块名
    // library: ['vue', 'element-ui'],
    // echarts: ['echarts'],
  },
  output: {
    filename: '[name]_[hash:10].dll.js', // 输出的文件名
    path: resolve(__dirname, 'build/dll'), // 输出的目录
    library: '[name]_[hash:10]', // 生成的库名
  },
  plugins: [
    new CleanWebpackPlugin({
      path: './build',
    }),
    new webpack.DllPlugin({
      name: '[name]_[hash:10]', // 此名需要与库名一致
      path: resolve(__dirname, 'build/dll/manifest.json'), // 输出的映射文件位置
    }),
  ],
  mode: 'production',
}
