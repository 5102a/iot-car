import Vue from 'vue'
import VueRouter from 'vue-router'
import Layout from '../views/Layout.vue'
import Login from '../views/Login.vue'
import Home from '../views/main/Home.vue'
const Config = () => import(/* webpackChunkName: "group-foo" */ '../views/main/Config.vue')


Vue.use(VueRouter)

const routes = [
  {
    path: '/login',
    name: 'login',
    component: Login,
    meta: {
      isPublic: true,
    },
  },
  {
    path: '/',
    name: 'main',
    redirect: '/login',
  },
  {
    path: '/layout',
    name: 'layout',
    component: Layout,
    children:[
      {
        path: '/home',
        name: 'home',
        component: Home,
        meta: {
          keepAlive: true,
        },
      },
      {
        path: '/config',
        name: 'config',
        component: Config,
        meta: {
          keepAlive: true,
        },
      }
    ]
  },
]

const router = new VueRouter({
  routes,
})

// 路由守卫
router.beforeEach((to, from, next) => {
  // 验证登录
  if (!to.meta.isPublic && !localStorage.getItem('iotc_token')) {
    return next('/login')
  }
  next()
})
export default router
