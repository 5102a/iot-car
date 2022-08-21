<template>
  <html class="main" style="width:100vw;height:100vh;">
    <canvas id="canvas"></canvas>
    <h3 class="title">5102物联网控制平台</h3>
    <div class="login-container">
      <form action="">
        <ul class="login-check">
          <li class="login-title">登录</li>
          <li class="login-username">
            <label class="iconfont icon-wo" for="username"></label
            ><input id="username" v-model="model.username" />
          </li>
          <li class="login-password">
            <label class="iconfont icon-suo" for="password"></label
            ><input id="password" type="password" v-model="model.password" />
          </li>
          <li class="login-button">
            <button @click.prevent="login">
              登录
            </button>
            <button @click.prevent="register">
              注册
            </button>
          </li>
        </ul>
      </form>
    </div>
  </html>
</template>

<script>
import md5 from 'js-md5'
export default {
  data() {
    return {
      model: {},
    }
  },
  methods: {
    // 登录
    async login() {
      try {
        var res = await this.$http.post('/login', {
          user: this.model.username,
          password: md5(this.model.password),
        })
      } catch (error) {
        return this.$message.error(`登录失败！`)
      }
      if (res.data.state === 0 && res.data.token) {
        localStorage.setItem('iotc_token', res.data.token)
        localStorage.setItem('iotc_user', this.model.username)
        this.$message.success(res.data.message)
        this.$router.push('/home')
      } else {
        this.$message.warning(res.data.message)
      }
    },
    // 注册账号
    async register(e) {
      e.preventDefault()
      try {
        var res = await this.$http.post('/register', {
          user: this.model.username,
          password: md5(this.model.password),
        })
      } catch (error) {
        return this.$message.error(`注册失败！`)
      }
      if (res.data.state === 0) {
        this.$message.success(res.data.message)
      } else {
        this.$message.warning(res.data.message)
      }
      return false
    },
    bgd() {
      ///////////canvas参数
      let can = document.querySelector('#canvas')
      let h = document.querySelector('html')
      can.width = h.offsetWidth
      can.height = h.offsetHeight
      //创建画布
      let ctx = can.getContext('2d')
      let lastX = 0,
        lastY = 0,
        clientX = 0,
        clientY = 0,
        count = 0
      let firstX,
        firstY,
        isDown = false

      /////////////////////监听html的鼠标移动
      let pointX, pointY, moveX, moveY //生成点的位置
      h.addEventListener('mousemove', (e) => {
        ///用于控制全局点跟随鼠标方向移动
        clientX = e.clientX ///记录鼠标当前位置
        clientY = e.clientY
        ////触发频率及按下条件
        if (count > 5 && isDown) {
          //向点集合添加点，创建点
          pointX = e.clientX + (Math.random() - 0.5) * 100
          pointY = e.clientY + (Math.random() - 0.5) * 100
          arrPoints.push(createPoint(pointX, pointY))
          const len = arrPoints.length
          if (len > 100) {
            //最多存在100个点
            for (let i = 0; i < len - 100; i++) {
              setTimeout(() => {
                arrPoints.splice(50, 1) //从70~100+ 作为变动点
              }, Math.random() * 200)
            }
          }
          count = 0
        }
        count++
        if (!firstX && !firstY) {
          //第一次才进来
          firstX = e.clientX //鼠标第一次进入窗口的坐标
          firstY = e.clientY
          lastX = e.clientX //第一次==最后一次
          lastY = e.clientY
        } else {
          //之后
          moveX = e.clientX - lastX //当前位置-上一次位置，作为点需要移动的系数
          moveY = e.clientY - lastY
          arrPoints.forEach((item) => {
            //遍历每个点，使每个点随鼠标移动
            item.speedX += moveX / (800 * ((Math.random() / 10) * 9 + 0.1)) //计算速度
            item.speedY += moveY / (800 * ((Math.random() / 10) * 9 + 0.1))
          })
          lastX = e.clientX
          lastY = e.clientY
        }
      })

      //////////监听画布上的点击
      can.addEventListener('mousedown', (e) => {
        isDown = true
        const downX = e.clientX + (Math.random() - 0.5) * 80
        const downY = e.clientY + (Math.random() - 0.5) * 80
        arrPoints.push(createPoint(downX, downY)) //没移动时按下也产生一个点
      })
      //////////监听在画布上的鼠标松开
      can.addEventListener('mouseup', () => {
        isDown = false
      })
      //记录所有点数据
      let arrPoints = []
      //获取随机颜色  ///太花了不要
      // function getColor(a = [255, 255], r = [0, 255], g = [0, 255], b = [0, 255]) {
      //   r = Math.random() * (r[1] - r[0]) + r[0]
      //   g = Math.random() * (g[1] - g[0]) + g[0]
      //   b = Math.random() * (b[1] - b[0]) + b[0]
      //   a = Math.random() * (a[1] - a[0]) + a[0]
      //   return `rgba(${~~r},${~~g},${~~b},${~~a})`
      // }
      ///////创建一个点
      function createPoint(x1, y1) {
        let curW, curH
        const color = '#30bbdd'
        const r = ~~(5 + (Math.random() - 0.5) * 2) //圆半径
        const speedX = (((Math.random() - 0.5) * 1) / 5).toFixed(4) * 4 //点的初始速度
        const speedY = (((Math.random() - 0.5) * 1) / 5).toFixed(4) * 4
        if (x1 == undefined) {
          //随机生成点
          const cw = ~~(can.width * Math.random()) ///随机位置生成点
          const ch = ~~(can.height * Math.random())
          curW = cw + speedX
          curH = ch + speedY
        } else {
          //给定位置生成点
          curW = x1 + speedX
          curH = y1 + speedY
        }
        //开始画图
        ctx.beginPath()
        ctx.arc(curW, curH, r, 0, Math.PI * 2)
        ctx.fillStyle = color
        ctx.closePath()
        ctx.fill()
        return {
          curW,
          curH,
          color,
          r,
          speedX,
          speedY,
          distanceX: curW, //用于记录移动
          distanceY: curH,
        }
      }

      //计算点与点距离
      function getDistance(x1, y1, x2, y2) {
        return ~~Math.sqrt(
          Math.pow(Math.abs(x1 - x2), 2) + Math.pow(Math.abs(y1 - y2), 2)
        )
      }
      //生成全部连线
      function putLines(arrPoints) {
        const len = arrPoints.length
        for (let i = 0; i < len; i++) {
          //循环所有点，通过插入算法
          for (let j = len - 1; j >= 0; j--) {
            const p1 = arrPoints[i], //记录2个点
              p2 = arrPoints[j],
              disPoint = getDistance(p1.curW, p1.curH, p2.curW, p2.curH)
            /////判断距离来决定连线       ///且圆属性邻居圆
            if (
              disPoint > p1.r + p2.r &&
              disPoint < 100 &&
              p1.r - 1 < p2.r < p1.r + 1
            ) {
              ctx.beginPath()
              ctx.moveTo(p1.curW, p1.curH)
              ctx.lineTo(p2.curW, p2.curH)
              ctx.strokeStyle = '#30bbdd'
              ctx.lineWidth = (p1.r / disPoint) * 10 //线宽
              ctx.closePath()
              ctx.stroke()
            }
          }
        }
      }

      //检测全部点，更新
      function putPoints(arrPoints) {
        arrPoints.forEach((item, i) => {
          //判断当前点的速度是否到达下限，重新计算速度
          if (item.speedX > 0.01 && item.speedY > 0.01) {
            item.speedX -= item.speedX / 100 //减速效果
            item.speedY -= item.speedY / 100
          }
          item.curW += item.speedX
          item.curH += item.speedY

          if (
            item.curW > can.width ||
            item.curH > can.height ||
            item.curW < 0 ||
            item.curH < 0
          ) {
            //判断点是否出边界
            arrPoints.splice(i, 1, createPoint()) //删除点,再重新随机生成
          }
          ctx.beginPath()
          ctx.arc(item.curW, item.curH, item.r, 0, 2 * Math.PI)
          ctx.fillStyle = '#10a0ee'
          ctx.closePath()
          ctx.fill()
        })
      }

      /////////初始化全部点，生成给定数量点
      function putPointsInit(num) {
        ctx.globalCompositeOperation = 'destination-over' //覆盖叠加模式
        ctx.globalAlpha = 0.7 ///全局透明度
        for (let i = 0; i < num; i++) {
          arrPoints.push(createPoint())
        }
      }

      function render() {
        ///渲染函数
        ctx.clearRect(0, 0, can.width, can.height) //更新前清屏再绘
        // can.width = h.offsetWidth //实时监听html的变换，可有可无
        // can.height = h.offsetHeight
        putPoints(arrPoints) //更新点位置
        putLines(arrPoints) //更新线
        window.requestAnimationFrame(render) ///尾递归循环
      }

      //-----------启动-------------//
      putPointsInit(80) //初始化点，生成点
      window.requestAnimationFrame(render) ///渲染之前调用
    },
  },
  mounted() {
    this.bgd()
  },
}
</script>

<style scoped>
#canvas {
  position: absolute;
  top: 0;
  left: 0;
  z-index: 1;
  overflow: hidden;
}
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

li {
  list-style: none;
}

.title {
  text-align: center;
  margin: 5vh 0;
  color: skyblue;
  font-size: 24px;
  text-shadow: 0.125rem 0.125rem 0.125rem #999;
}

.main {
  position: relative;
  /* background: url('../assets/img/login.jpg') no-repeat;
  background-position: center;
  background-size: 100% 100%; */
  overflow: hidden;
  width: 100vw;
  height: 100vh;
}

.login-container {
  position: absolute;
  z-index:1000;
  width: 18.75rem;
  height: 25rem;
  opacity: 0.85;
  top: 50%;
  left: 50%;
  margin-left: -9.375rem;
  margin-top: -12.5rem;
  padding-top: 0.625rem;
  background: linear-gradient(
    45deg,
    #0ac,
    rgb(35, 99, 218) 95%,
    rgba(255, 255, 255, 0) 0.3125rem
  );
  transition-delay: 300ms;
  border-radius: 0.1875rem;
  box-shadow: -0.9375rem 0.9375rem 0.625rem 0.0625rem #0088dd55;
  text-align: center;
  color: rgba(200, 200, 200, 0.5);
}

form {
  display: flex;
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  margin: auto;
  width: 17.5rem;
  height: 23.75rem;
  justify-content: center;
  flex-wrap: wrap;
  z-index: 1;
}

.login-title {
  padding: 1.25rem;
  user-select: none;
  color: #fff;
}
.login-username,
.login-password {
  color: #fff;
}
form label {
  position: relative;
  top: 0.1875rem;
  width: 1.5625rem;
  transition-duration: 1s;
}

form li:not(:nth-child(1)) {
  position: relative;
  padding: 0.9375rem;
  z-index: 2;
  width: 15.625rem;
}

form input {
  justify-self: center;
  background-color: rgba(54, 172, 240, 0.89);
  border-radius: 0.1875rem;
  margin-left: 0.3125rem;
  width: 9.375rem;
  height: 1.5625rem;
  flex: 1;
  font-size: 1rem;
  transition-duration: 1s;
  outline: none;
  border: 0.125rem solid #999;
  opacity: 0.5;
  transition-delay: 200ms;
  transition-timing-function: cubic-bezier(0.075, 0.82, 0.165, 1);
}

form input:focus {
  opacity: 1;
  border: 0.125rem solid rgb(194, 189, 189);
}

.login-button button {
  user-select: none;
  outline: none;
  cursor: pointer;
  margin-top: 1.875rem;
  width: 7.5rem;
  height: 1.875rem;
  border-radius: 1.25rem;
  border: 0.125rem solid #cef;
  color: #fff;
  font-weight: 700;
  transition-duration: 500ms;
  background: linear-gradient(0deg, #0af, #0af);
}

.login-button button:hover {
  transform: scale(1.1, 1.1);
}

.login-button button:active {
  transition-duration: 100ms;
  transform: scale(0.9, 0.9);
}
html {
  background: linear-gradient(45deg, #08a, #04a);
  overflow: hidden;
}
</style>
