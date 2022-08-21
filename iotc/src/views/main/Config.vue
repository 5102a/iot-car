<template>
  <div class="config-main">
    <div class="main-top">
      <el-card class="box-card" :body-style="{ color: '#eee' }">
        <div
          slot="header"
          :class="`card-header ${mqttIsConnected ? 'is-connected' : ''}`"
        >
          <span>MQTT服务器</span>
        </div>
        <div class="card-item">
          <div class="card-item-left">
            <ul>
              <li>
                复位云控制连接：<el-button
                  type="primary"
                  size="mini"
                  :loading="loading.cloudControl"
                  @click="handleResetCloudControl"
                  ><i class="iconfont icon-fuwei1"></i
                ></el-button>
              </li>
              <li>
                需同步的数据：
                <el-select
                  class="sync-select"
                  v-model="syncData"
                  multiple
                  placeholder="请选择同步的数据"
                >
                  <el-option-group
                    v-for="group in syncGroup"
                    :key="group.label"
                    :label="group.label"
                  >
                    <el-option
                      v-for="item in group.data"
                      :key="item.label"
                      :label="item.label"
                      :value="item.value"
                    >
                    </el-option>
                  </el-option-group>
                </el-select>
                <el-button
                  type="primary"
                  size="mini"
                  style="margin-left:10px;"
                  @click="handleChangeSyncData"
                  >保存</el-button
                >
              </li>
            </ul>
          </div>
          <div class="card-item-right"></div>
        </div>
      </el-card>
      <el-card class="box-card">
        <div
          slot="header"
          :class="`card-header ${controlIsConnected ? 'is-connected' : ''}`"
        >
          <span>云控制服务器</span>
        </div>
        <div class="card-item">
          <div class="card-item-left">
            <ul>
              <li>
                Bootloader操作：<el-button
                  type="warning"
                  size="mini"
                  @click="handleEntryBootloader"
                  >进入引导程序</el-button
                >
                <el-button
                  type="primary"
                  size="mini"
                  :loading="loading.entryApp"
                  @click="handleQuitBootloader"
                  >退出并进入应用程序</el-button
                >
              </li>
              <li>
                在线固件升级：<el-upload
                  class="upload"
                  ref="upload"
                  action="https://wlw.5102it.cn/updatefirmware/"
                  :auto-upload="false"
                  :multiple="false"
                  :show-file-list="true"
                  :limit="1"
                  :on-success="handleUploadSuccess"
                  :on-error="handleUploadError"
                  :file-list="fileList"
                  :http-request="uploadFile"
                >
                  <el-button slot="trigger" size="small" type="primary"
                    >选取固件</el-button
                  >
                  <el-button
                    style="margin-left: 10px;"
                    size="small"
                    type="success"
                    :loading="loading.upload"
                    @click="submitUpload"
                    >{{
                      loading.upload ? '更新中...' : '更新到设备'
                    }}</el-button
                  >
                </el-upload>
              </li>
            </ul>
          </div>
          <div class="card-item-right"></div>
        </div>
      </el-card>
    </div>
    <div class="main-bottom">
      <el-card class="box-card" :body-style="{ color: '#eee' }">
        <div slot="header" class="card-header">
          <span>默认配置</span>
          <el-button
            style="float: right; padding: 3px 0"
            type="text"
            @click="handleChangeDefaultConfig"
            >保存</el-button
          >
        </div>
        <div class="config-body">
          <div class="camera-left">
            <ul>
              <li>
                <i class="iconfont icon-14"></i>对比度：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_contrast"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option
                    v-for="item in selectList"
                    :key="item"
                    :label="item + '级'"
                    :value="item"
                    >{{ item }} 级
                  </el-option>
                </el-select>
              </li>
              <li>
                <i class="iconfont icon-saturation"></i>饱和度：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_saturation"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option
                    v-for="item in selectList"
                    :key="item"
                    :label="item + '级'"
                    :value="item"
                    >{{ item }} 级
                  </el-option>
                </el-select>
              </li>
              <li>
                <i class="iconfont icon-heibaipingheng"></i>白平衡：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_light_mode"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option
                    v-for="item in selectList"
                    :key="item"
                    :label="item + '级'"
                    :value="item"
                    >{{ item }} 级
                  </el-option>
                </el-select>
              </li>
              <li>
                <i class="iconfont icon-football_name"></i>模式：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_mode"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option label="连续" :value="0"> </el-option>
                </el-select>
              </li>
              <li>
                <i class="iconfont icon-geshi_tupianjpg"></i>格式：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_workmode"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option label="JPG" :value="1"> </el-option>
                </el-select>
              </li>
            </ul>
          </div>
          <div class="camera-right">
            <ul>
              <li>
                <i class="iconfont icon-brightj2"></i>亮度：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_brightness"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option
                    v-for="item in selectList"
                    :key="item"
                    :label="item + '级'"
                    :value="item"
                    >{{ item }} 级
                  </el-option>
                </el-select>
              </li>
              <li>
                <i class="iconfont icon-texiao"></i>特效：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_effect"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option
                    v-for="(item, index) in camera_effectList"
                    :key="index"
                    :label="item"
                    :value="index"
                  >
                  </el-option>
                </el-select>
              </li>
              <li>
                <i class="iconfont icon-puguang"></i>曝光：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_autoexposure_level"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option
                    v-for="item in selectList"
                    :key="item"
                    :label="item + '级'"
                    :value="item"
                    >{{ item }} 级
                  </el-option>
                </el-select>
              </li>
              <li>
                <i class="iconfont icon-tiaoxingma"></i>彩条：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_color_bar"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option label="关闭" :value="0"> </el-option>
                  <el-option label="开启" :value="1"> </el-option>
                </el-select>
              </li>
              <li>
                <i class="iconfont icon-quanping_huaban1"></i>尺寸：<el-select
                  :class="'select-item '"
                  v-model="carState.camera_size"
                  placeholder="请选择"
                  popper-class="option-list"
                >
                  <el-option
                    v-for="(item, index) in camera_size"
                    :key="index"
                    :label="item"
                    :value="index"
                  >
                  </el-option>
                </el-select>
              </li>
            </ul>
          </div>
          <div class="duoji-setting">
            <ul>
              <li>
                <i class="iconfont icon-duoji1"></i>一维舵机角度：
                <el-slider
                  class="duoji"
                  v-model="carState.cur_duoji_1_angle"
                  :min="-135"
                  :max="135"
                  :step="5"
                  show-tooltip
                >
                </el-slider>
              </li>
              <li>
                <i class="iconfont icon-chuangyipingtai_duoji"></i
                >二维舵机角度：
                <el-slider
                  class="duoji"
                  v-model="carState.cur_duoji_2_angle"
                  :min="-45"
                  :max="135"
                  :step="5"
                  show-tooltip
                >
                </el-slider>
              </li>
              <li>
                <i class="iconfont icon-xuanzhuan"></i>设置旋转速度：
                <el-slider
                  class="duoji"
                  v-model="carState.set_dir_speed_whole"
                  :min="0"
                  :max="200"
                  :step="1"
                  show-tooltip
                >
                </el-slider>
              </li>
              <li>
                <i class="iconfont icon-shipin2"></i>默认开启视频：
                <el-switch
                  class="img-open"
                  :inactive-value="1"
                  :active-value="0"
                  v-model="carState.camera_is_abort"
                >
                </el-switch>
              </li>
            </ul>
          </div>
        </div>
      </el-card>
      <el-card class="box-card-mini">
        <div slot="header">
          <span>设备管理</span>
          <el-button
            style="float: right; padding: 3px 0"
            type="text"
            @click="handleAddDevice"
            >添加设备</el-button
          >
        </div>
        <div class="device-manage">
          <div class="device-manage-left">
            已有设备：
            <el-card class="device-manage-card">
              <ul>
                <li v-for="(car, index) in deviceList" :key="index">
                  <div class="car-name" @click="handleShowDevice(index)">
                    {{ car.name }}
                  </div>
                  <el-button
                    style="flex:1; padding: 3px 0"
                    type="text"
                    @click="handleDeleteDevice(car.secret, index)"
                    >删除</el-button
                  >
                </li>
              </ul>
            </el-card>
          </div>
          <div class="device-manage-right">
            设备详情：
            <el-button
              size="mini"
              type="text"
              style="float:right; padding: 5px"
              @click="handleSaveDevice('edit')"
              >保存设备</el-button
            >
            <el-card class="device-manage-card">
              <div>
                <ul>
                  <li>
                    设备名：<el-input
                      size="mini"
                      class="car-manage-input"
                      v-model="curDevice.name"
                      placeholder="请输入设备名"
                    ></el-input>
                  </li>
                  <li>
                    设备密钥：<el-input
                      size="mini"
                      class="car-manage-input"
                      v-model="curDevice.secret"
                      placeholder="请输入设备密钥"
                    ></el-input>
                  </li>
                </ul>
              </div>
            </el-card>
          </div>
        </div>
      </el-card>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      camera_effectList: [
        '正常',
        '负片',
        '黑白',
        '偏红色',
        '偏绿色',
        '偏蓝色',
        '复古',
      ],
      camera_size: [
        '160*120', //QQVGA
        '176*144', //QCIF
        '320*240', //QVGA
        '400*240', //WQVGA 3
        '352*288', //CIF
        '640*480', //VGA 5
        '800*600', //SVGA 6
        '1024*768', //XGA
        '1280*800', //WXGA
        '1280*960', //XVGA  9
        '1440*900', //WXGA+
        '1280*1024', //SXGA
        '1600*1200', //UXGA	12
      ],
      selectList: [0, 1, 2, 3, 4],
      mqttIsConnected: false,
      controlIsConnected: false,
      // 小车数据
      carState: {
        camera_is_abort: 1,
        // 舵机
        cur_duoji_1_angle: 0,
        cur_duoji_2_angle: 0,
        set_dir_speed_whole: 20,
        // 相机设置
        camera_mode: 0, // 模式
        camera_workmode: 1, // 格式
        camera_contrast: 2, // 对比度
        camera_saturation: 4, // 饱和度
        camera_effect: 0, // 特效
        camera_autoexposure_level: 0, // 曝光
        camera_light_mode: 4, // 白平衡
        camera_brightness: 3, // 亮度
        camera_color_bar: 0, // 彩条
        camera_size: 5, // 尺寸
      },
      fileList: [],
      loading: {
        entryApp: false,
        upload: false,
        cloudControl: false,
      },
      // 同步的数据字段
      syncData: [
        'cur_speed_whole',
        'cur_speed_lq',
        'cur_speed_rq',
        'cur_speed_lh',
        'cur_speed_rh',
        'cur_duoji_1_angle',
        'cur_duoji_2_angle',
        'camera_mode',
        'camera_workmode',
        'camera_contrast',
        'camera_saturation',
        'camera_effect',
        'camera_autoexposure_level',
        'camera_light_mode',
        'camera_brightness',
        'camera_color_bar',
        'camera_size',
        'camera_frame',
        'camera_is_abort',
        'history_speed',
        'history_frame',
      ],
      // 同步的字段分组
      syncGroup: [
        {
          label: '车速数据',
          data: [
            { value: 'cur_speed_whole', label: '整体车速' },
            { value: 'cur_speed_lq', label: '左前轮车速' },
            { value: 'cur_speed_rq', label: '右前轮车速' },
            { value: 'cur_speed_lh', label: '左后轮车速' },
            { value: 'cur_speed_rh', label: '右后轮车速' },
          ],
        },
        {
          label: '舵机数据',
          data: [
            { value: 'cur_duoji_1_angle', label: '一维舵机角度' },
            { value: 'cur_duoji_2_angle', label: '二维舵机角度' },
          ],
        },
        {
          label: '摄像头数据',
          data: [
            { value: 'camera_mode', label: '模式' },
            { value: 'camera_workmode', label: '格式' },
            { value: 'camera_contrast', label: '对比度' },
            { value: 'camera_saturation', label: '饱和度' },
            { value: 'camera_effect', label: '特效' },
            { value: 'camera_autoexposure_level', label: '曝光' },
            { value: 'camera_light_mode', label: '白平衡' },
            { value: 'camera_brightness', label: '亮度' },
            { value: 'camera_color_bar', label: '彩条' },
            { value: 'camera_size', label: '尺寸' },
            { value: 'camera_frame', label: '帧率' },
            { value: 'camera_is_abort', label: '视频状态' },
          ],
        },
        {
          label: '历史数据',
          data: [
            { value: 'history_speed', label: '小车车速' },
            { value: 'history_frame', label: '视频帧率' },
          ],
        },
      ],
      deviceList: [],
      curDevice: {
        name: '',
        secret: '',
      },
    }
  },
  mounted() {
    // 获取同步数据
    this.getSyncData()
    // 获取默认配置
    this.getDefaultConfig()

    // 获取设备列表
    this.getDeviceList()
    setInterval(() => {
      let state = sessionStorage.getItem('iotc_state')
      if (state) {
        state = JSON.parse(state)
        if (!this.mqttIsConnected && state.mqttIsConnected) {
          this.loading.upload = false
          this.loading.entryApp = false
        }
        this.mqttIsConnected = state.mqttIsConnected
        this.controlIsConnected = state.controlIsConnected
      }
    }, 1000)
  },
  methods: {
    /* 处理 */
    // 处理响应通知
    showMessage(state, message, show = false) {
      switch (state) {
        case 0: // 成功
          show && this.$message.success(message)
          break
        case -1: // 登录过期
          break
        default:
          // 其他
          show && this.$message.warning(message)
          break
      }
    },
    /* 请求 */
    // 通用请求
    async commonRequest(url, show = false, data = {}, method = 'post') {
      const user = localStorage.getItem('iotc_user') || ''
      const secret = localStorage.getItem('iotc_secret') || ''
      const preData = {
        user,
        secret,
      }
      try {
        var res = await this.$http[method](url, Object.assign(preData, data))
      } catch (error) {}
      this.showMessage(res.data.state, res.data.message, show)
      return res
    },
    // 上传文件
    async uploadFile(req) {
      const forms = new FormData()
      const user = localStorage.getItem('iotc_user') || ''
      const secret = localStorage.getItem('iotc_secret') || ''
      const configs = {
        headers: { 'Content-Type': 'multipart/form-data' },
      }
      forms.append('file', req.file)
      forms.append('user', user)
      forms.append('secret', secret)
      if (this.mqttIsConnected) {
        await this.commonRequest(`/command/r-2`, false, { data: '1' })
        setTimeout(async () => {
          await this.commonRequest(`/command/r-3`, false, { data: '1' })
          try {
            var res = await this.$http.post('/upload/firmware', forms, configs)
          } catch (error) {
            this.$message.warning('上传失败')
          }
        }, 18000)
      } else {
        await this.commonRequest(`/command/r-3`, false, { data: '1' })
        try {
          var res = await this.$http.post('/upload/firmware', forms, configs)
        } catch (error) {
          this.$message.warning('上传失败')
        }
      }
      this.showMessage(res.data.state, res.data.message, true)
    },
    // 获取需同步的数据
    async getSyncData() {
      const res = await this.commonRequest(`/get/sync`)
      if (res.data.data) {
        this.syncData = res.data.data.split(',') || []
      }
    },
    // 获取默认配置
    async getDefaultConfig() {
      const res = await this.commonRequest(`/get/default`)
      if (res.data.data) {
        Object.assign(this.carState, JSON.parse(res.data.data) || {})
      }
    },
    // 设备管理
    async deviceManage(type, secret, name) {
      const res = await this.commonRequest(`/manage/${type}`, true, {
        secret: secret,
        name: name,
      })
      if (res.data.data) {
        this.deviceList = JSON.parse(res.data.data) || []
      }
    },
    // 获取设备列表
    async getDeviceList() {
      const res = await this.commonRequest(`/public/deviceList`)
      if (res.data.data) {
        // console.log(res.data)
        this.deviceList = JSON.parse(res.data.data) || []
      }
    },
    /* 事件 */
    // 展示设备详情
    handleShowDevice(index) {
      this.curDevice.name = this.deviceList[index].name
      this.curDevice.secret = this.deviceList[index].secret
    },
    // 保存设备
    handleSaveDevice(type) {
      const dev = this.curDevice
      if (dev.secret && dev.name) {
        this.deviceManage(type, dev.secret, dev.name)
      } else {
        this.$message.warning('字段不符合,请重新填写')
      }
    },
    // 添加设备
    handleAddDevice() {
      this.deviceList.push({
        name: '',
        secret: '',
      })
    },
    // 删除设备
    handleDeleteDevice(secret, index) {
      if (secret) {
        this.deviceManage('delete', secret)
      }
      this.deviceList.splice(index, 1)
    },

    // 保存默认配置
    handleChangeDefaultConfig() {
      this.commonRequest('/modify/default', true, { data: this.carState })
    },
    // 修改同步的字段
    handleChangeSyncData() {
      this.commonRequest('/modify/sync', true, { data: this.syncData })
    },
    // 进入bootloader
    handleEntryBootloader() {
      if (!this.controlIsConnected) {
        return this.$message.warning('云控制服务器未连接,无法执行指令')
      }
      this.commonRequest(`/command/r-2`, true, {
        data: '1',
      })
      setTimeout(() => {
        this.commonRequest(`/command/c-0`, true, {
          data: '1',
        })
      }, 18000)
      setTimeout(() => {
        this.$message.warning('请等待重新连接云控制服务器')
      }, 5000)
    },
    // 退出bootloader
    handleQuitBootloader() {
      if (!this.controlIsConnected) {
        return this.$message.warning('云控制服务器未连接,无法执行指令')
      }
      if (this.mqttIsConnected) {
        return this.$message.warning('请先进入Bootloader模式')
      }
      this.loading.entryApp = true
      this.commonRequest(`/command/a-1`, true, {
        data: '0',
      })
    },
    // 重置云控制服务器连接
    handleResetCloudControl() {
      // mqtt未连接
      if (!this.mqttIsConnected) {
        return this.$message.warning('MQTT服务器未连接,无法执行指令')
      }
      sessionStorage.setItem('reset_wifi2', '1')
      // 重置云控制服务器连接
    },
    // 上传固件更新小车
    submitUpload() {
      if (!this.controlIsConnected) {
        return this.$message.warning('云控制服务器未连接,无法更新')
      }
      this.loading.upload = true
      this.$refs.upload.submit()
    },
    // 上传成功,回调
    handleUploadSuccess() {
      // 清空列表
      this.fileList = []
      // this.loading.upload = false
    },
    // 上传失败回调
    handleUploadError() {
      // this.loading.upload = false
    },
  },
}
</script>

<style scoped>
  .config-body /deep/ .el-input.el-input--suffix,
  .config-body /deep/ .el-input__inner,
  .config-body /deep/ .el-select-dropdown.el-popper.option-list{
    height: 22px !important;
  }
.config-main {
  height: 100vh;
  min-width: 568px !important;
}
.main-top {
  height: 50vh;
  min-width: 568px !important;
  padding: 10px;
  display: flex;
}
.main-bottom {
  position: relative;
  height: 35vh;
  min-width: 568px !important;
  padding: 10px;
  display: flex;
}
.box-card {
  flex: 4;
  background-color: #3388ee33;
  margin: 0 10px;
  color: #eee;
}
.box-card-mini {
  flex: 2;
  background-color: #3388ee33;
  margin: 0 10px;
  color: #eee;
}
.device-manage-card {
  background-color: #3388ee33;
  margin: 5px 0px;
  color: #eee;
  height: 18vh;
  overflow: auto;
}

.device-manage-left ul > li {
  border: 1px solid #aaa;
  border-bottom: 0px solid #aaa;
  box-sizing: border-box;
  display: flex;
  align-items: center;
}
.device-manage-left ul > li:last-child {
  border-bottom: 1px solid #aaa;
}
.box-card-mini /deep/ .el-card__body {
  padding: 10px !important;
}
.car-name {
  cursor: pointer;
  height: 15px;
  line-height: 15px;
  flex: 3;
  padding: 0px 5px;
}

.car-manage-input /deep/ .el-input__inner {
  background: #2233aa !important;
  color: #eee;
}

.device-manage {
  display: flex;
}
.device-manage-left,
.device-manage-right {
  flex: 1;
  margin: 0 5px;
}
.camera-left,
.camera-right {
  width: 30%;
  float: left;
}
.duoji-setting {
  width: 40%;
  float: left;
  height: 100%;
  display: flex;
  justify-content: center;
  color: #aaa;
  font-size: 14px;
}
.duoji-setting > ul > li {
  display: flex;
  justify-content: center;
  align-items: center;
}
.duoji-setting > ul > li > i {
  padding-right: 5px;
}

.duoji,
.img-open {
  width: 150px !important;
}
.card-item-left > ul > li,
.card-item-right > ul > li {
  display: flex;
  align-items: center;
  margin: 10px 0px;
}
.baud-rate {
  display: inline-block;
  width: 320px;
}
.baud-rate /deep/ .el-input__inner {
  background: #2233aa !important;
  color: #eee;
}
.upload {
  display: flex;
  align-items: center;
  width: 230px;
  flex-wrap: wrap;
}
.sync-select {
  width: 400px;
}
.sync-select /deep/ .el-input__inner {
  background: #2233aa !important;
}
.is-connected {
  background-color: #0118ca9f !important;
}
.card-header {
  padding: 18px 20px;
}
.box-card /deep/ .el-card__header {
  padding: 0px !important;
}
</style>
