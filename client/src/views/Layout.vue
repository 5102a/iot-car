<!--
 * @Author: your name
 * @Date: 2020-10-01 11:12:40
 * @LastEditTime: 2021-02-28 18:40:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \internet-of-things-control\iotc\src\views\Layout.vue
-->
<template>
  <div class="layout">
    <div class="container-main">
      <header class="container-header">
        <el-menu
          default-active="home"
          class="nav-menu"
          mode="horizontal"
          text-color="#ffffffaa"
          active-text-color="#ffd04baa"
          router
        >
          <div class="logo-img">
            <img src="@/assets/img/logo.png" alt="logo" />
          </div>
          <el-menu-item index="home">首页</el-menu-item>
          <el-menu-item index="config">配置中心</el-menu-item>
        </el-menu>
      </header>
      <div class="container-body">
        <keep-alive>
          <router-view v-if="$route.meta.keepAlive"></router-view>
        </keep-alive>
        <router-view v-if="!$route.meta.keepAlive"></router-view>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      var: '',
    }
  },
}
</script>

<style scoped>
@media screen and (min-width: 1024px) {
  * {
    -moz-user-select: none; /* Firefox私有属性 */
    -webkit-user-select: none; /* WebKit内核私有属性 */
    -ms-user-select: none; /* IE私有属性(IE10及以后) */
    -khtml-user-select: none; /* KHTML内核私有属性 */
    -o-user-select: none; /* Opera私有属性 */
    user-select: none; /* CSS3属性 */
  }
  .container-header .nav-menu .el-menu-item {
    background: transparent !important;
  }
  .container-header .nav-menu .el-menu-item:hover {
    background: #151a6755 !important;
  }
  .container-main {
    width: 100vw;
    height: 100vh;
    min-width: 70rem !important;
    position: relative;
    background: url('../assets/img/bg.jpg') no-repeat !important;
    background-size: 100% 100% !important;
    background-position: center !important;
  }
  .container-header {
    width: 100vw;
    height: 60px;
    position: relative;
  }
  .container-body {
    width: 100vw;
    height: calc(100vh - 60px);
    position: relative;
  }
  .nav-menu {
    background: url('../assets/img/head_bg.png') no-repeat;
    background-size: 100% 100%;
  }
  .logo-img {
    height: 60px;
    width: 240px;
    float: left;
    outline: none;
  }
  .logo-img img {
    height: 60px;
  }
}
@media screen and (orientation: landscape) and (min-width: 568px) and (max-width: 1024px) {
  * {
    -moz-user-select: none; /* Firefox私有属性 */
    -webkit-user-select: none; /* WebKit内核私有属性 */
    -ms-user-select: none; /* IE私有属性(IE10及以后) */
    -khtml-user-select: none; /* KHTML内核私有属性 */
    -o-user-select: none; /* Opera私有属性 */
    user-select: none; /* CSS3属性 */
  }
  .container-header .nav-menu .el-menu-item:hover {
    background: #151a6755 !important;
  }

  .container-main {
    width: 100vw;
    height: 100vh;
    min-width: 568px !important;
    position: relative;
    background: url('../assets/img/bg.jpg') no-repeat !important;
    background-size: 100% 100% !important;
    background-position: center !important;
  }
  .container-header {
    height: 0px;
  }
  .container-body {
    width: 100vw;
    height: 100vh;
    position: relative;
  }
}
</style>
