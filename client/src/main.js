import Vue from 'vue'
import App from './App.vue'
import router from './router'
import './plugins/element.js'
import './plugins/echars.js'
import './assets/css/style.css'
import axios from 'axios'

let timer


Vue.config.productionTip = false

Vue.prototype.$http = axios.create({
  baseURL:
    process.env.NODE_ENV === 'production'
      ? 'https://wlw.5102it.cn'
      : 'https://wlw.5102it.cn', //'http://localhost:4444'
  // baseURL: 'http://localhost',
  // baseURL: 'http://wlw.5102it.cn',
})

// 添加请求拦截器
Vue.prototype.$http.interceptors.request.use(function(config) {
  // 在发送请求之前做些什么
  // 设置token
  config.headers.common['Authorization'] = `Bearer ${localStorage.getItem(
    'iotc_token'
  ) || ''}`
  return config
})
// 添加响应拦截器
Vue.prototype.$http.interceptors.response.use(function(config) {
  // token过期
  if (config.data.state === -1 && !timer) {
    localStorage.removeItem('iotc_token')
    app.$message.warning('登录已过期,请重新登录！')
    app.$router.push('/login')
    timer = setTimeout(() => {
      timer = null
    }, 5000)
  }
  return config
})

const app = new Vue({
  router,
  render: (h) => h(App),
}).$mount('#app')
