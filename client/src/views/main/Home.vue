<template>
  <div class="home-main">
    <div class="main-container">
      <div class="left-aside">
        <div class="ls-item speed-whool">
          <i></i>
          <div id="speed-whool"></div>
        </div>
        <div class="ls-item speed-every">
          <i></i>
          <div id="speed-every"></div>
        </div>
      </div>
      <div class="main-body" style="color:#fff">
        <div class="ls-item main-body-top">
          <i></i>
          <div class="car-select">
            <el-select
              popper-class="car-select-item"
              v-model="curSelectCar"
              size="mini"
              placeholder="请选择连接的设备"
              @change="handleChangeCar"
            >
              <el-option
                v-for="car in carSelectList"
                :key="car.name"
                :label="car.name"
                :value="car.secret"
              >
              </el-option>
            </el-select>
          </div>
          <div
            class="car-state"
            :title="mqttIsConnected ? 'MQTT服务器已连接' : 'MQTT服务器未连接'"
          >
            <div
              :class="
                mqttIsConnected ? 'car-state-connected' : 'car-state-disconnect'
              "
            >
              <i
                :class="
                  'iconfont ' +
                    (mqttIsConnected ? 'icon-B' : 'icon-duankailianjie1')
                "
              ></i>
            </div>
          </div>
          <div
            class="car-state"
            :title="
              controlIsConnected ? '云控制服务器已连接' : '云控制服务器未连接'
            "
          >
            <i
              :class="
                `iconfont icon-zaixian ${
                  controlIsConnected
                    ? 'car-state-connected'
                    : 'car-state-disconnect'
                }`
              "
            ></i>
          </div>
          <div class="zhenglv" title="帧率">
            <i
              class="iconfont icon-frequency"
              style="color:yellow;padding-right:10px;"
            ></i
            >{{ curEchartsData.camera_frame }}&nbsp;FPS
          </div>
          <div
            class="openImg"
            :disabled="!propsIsOk['camera_is_abort']"
            :title="propsIsOk['camera_is_abort'] ? '' : '生效中'"
          >
            视频：
            <div
              :class="
                `img-state ${
                  curEchartsData.camera_is_abort ? 'cur-img-state' : ''
                }`
              "
              @click="handleChangeCameraState(1)"
            >
              关
            </div>
            <div
              :class="
                `img-state ${
                  curEchartsData.camera_is_abort ? '' : 'cur-img-state'
                }`
              "
              @click="handleChangeCameraState(0)"
            >
              开
            </div>
          </div>
        </div>
        <div class="ls-item img-container">
          <i></i>
          <div :class="curEchartsData.camera_is_abort ? 'offline' : 'online'">
            {{
              curEchartsData.camera_is_abort ? '●&nbsp;offLine' : '●&nbsp;Live'
            }}
          </div>
          <div
            :class="
              propsIsOk['camera_is_abort'] && hasReceiveImgData
                ? 'hidden'
                : 'is-disabled-0'
            "
          >
            {{
              curEchartsData.camera_is_abort ? '视频已断开' : '视频连接中...'
            }}
          </div>
          <img id="imgData" />
          <div class="downloadImg" title="保存图片" @click="handleSaveImg">
            <i class="iconfont icon-baocuntupian"></i>
          </div>
        </div>
        <div class="ls-item main-body-bottom">
          <i></i>
          <el-slider
            class="set-speed-whole"
            v-model="curEchartsData.set_dir_speed_whole"
            :min="0"
            :max="200"
            show-tooltip
            vertical
            height="120"
            :format-tooltip="(value) => `设置旋转速度:${value}cm/s`"
          >
          </el-slider>

          <el-slider
            class="duoji-2"
            v-model="curEchartsData.cur_duoji_2_angle"
            :min="-45"
            :max="135"
            show-tooltip
            vertical
            :step="1"
            height="120"
            :disabled="!propsIsOk['cur_duoji_2_angle']"
            :title="propsIsOk['cur_duoji_2_angle'] ? '' : '生效中'"
            :format-tooltip="
              (value) =>
                `二维:${value <= 0 ? `下旋${-value}` : `上旋${value}`}°`
            "
            @change="handleChangeDuoji('cur_duoji_2_angle')"
          >
          </el-slider>
          <div
            class="car-update"
            title="小车数据更新到云端"
            @click="handleSyncData('car')"
          >
            <i class="iconfont icon-iconset0349"></i>
          </div>
          <div
            class="cloud-sync"
            title="云端数据同步到小车"
            @click="handleSyncData('cloud')"
          >
            <i class="iconfont icon-ziyuan1412"></i>
          </div>
          <div class="drag-base"></div>
          <div id="drag-gun"></div>
          <div id="drag-pointer"></div>
          <div class="drag-mask"></div>
          <div
            class="turn-left"
            title="左旋"
            @mousedown="handleDirChange(1)"
            @mouseup="handleDirChange(0)"
            @touchstart="handleDirChange(1)"
            @touchend="handleDirChange(0)"
          >
            <i class="iconfont icon-zuozhuan"></i>
          </div>
          <div
            class="turn-right"
            title="右旋"
            @mousedown="handleDirChange(-1)"
            @mouseup="handleDirChange(0)"
            @touchstart="handleDirChange(-1)"
            @touchend="handleDirChange(0)"
          >
            <i class="iconfont icon-youzhuan"></i>
          </div>

          <el-slider
            class="duoji-1"
            v-model="curEchartsData.cur_duoji_1_angle"
            :min="-135"
            :max="135"
            :step="1"
            show-tooltip
            :disabled="!propsIsOk['cur_duoji_1_angle']"
            :title="propsIsOk['cur_duoji_1_angle'] ? '' : '生效中'"
            :format-tooltip="
              (value) =>
                `一维:${value <= 0 ? `左旋${-value}` : `右旋${value}`}°`
            "
            @change="handleChangeDuoji('cur_duoji_1_angle')"
          >
          </el-slider>
          <div class="speed-level">
            <div
              :class="
                `speed-level-stage ${speedLevel === 8 ? 'cur-speed-level' : ''}`
              "
              @click="handleSpeedLevel(8)"
            >
              低速
            </div>
            <div
              :class="
                `speed-level-stage ${speedLevel === 4 ? 'cur-speed-level' : ''}`
              "
              @click="handleSpeedLevel(4)"
            >
              中速
            </div>
            <div
              :class="
                `speed-level-stage ${speedLevel === 1 ? 'cur-speed-level' : ''}`
              "
              @click="handleSpeedLevel(1)"
            >
              高速
            </div>
          </div>
          <div title="舵机复位" class="duoji-reset" @click="handleDuojiReset">
            <i class="iconfont icon-fuwei"></i>
          </div>
        </div>
      </div>
      <div class="right-aside">
        <div class="rs-item">
          <i></i>
          <div
            :class="propsIsOk['cur_duoji_1_angle'] ? 'hidden' : 'is-disabled-1'"
          >
            生效中
          </div>
          <div
            :class="propsIsOk['cur_duoji_2_angle'] ? 'hidden' : 'is-disabled-2'"
          >
            生效中
          </div>
          <div id="duoji-angle"></div>
        </div>
        <div class="rs-item">
          <i></i>
          <div class="camera-state-title">摄像头状态</div>
          <div class="camera-state-content">
            <div class="camera-left">
              <ul>
                <li>
                  <i class="iconfont icon-14"></i>对比度：<el-select
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_contrast'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_contrast"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_contrast']"
                    :title="propsIsOk['camera_contrast'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_contrast')"
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
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_saturation'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_saturation"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_saturation']"
                    :title="propsIsOk['camera_saturation'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_saturation')"
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
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_light_mode'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_light_mode"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_light_mode']"
                    :title="propsIsOk['camera_light_mode'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_light_mode')"
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
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_mode'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_mode"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_mode']"
                    :title="propsIsOk['camera_mode'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_mode')"
                  >
                    <el-option label="连续" :value="0"> </el-option>
                  </el-select>
                </li>
                <li>
                  <i class="iconfont icon-geshi_tupianjpg"></i>格式：<el-select
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_workmode'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_workmode"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_workmode']"
                    :title="propsIsOk['camera_workmode'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_workmode')"
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
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_brightness'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_brightness"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_brightness']"
                    :title="propsIsOk['camera_brightness'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_brightness')"
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
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_effect'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_effect"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_effect']"
                    :title="propsIsOk['camera_effect'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_effect')"
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
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_autoexposure_level']
                          ? ''
                          : 'invalid')
                    "
                    v-model="curEchartsData.camera_autoexposure_level"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_autoexposure_level']"
                    :title="
                      propsIsOk['camera_autoexposure_level'] ? '' : '生效中'
                    "
                    @change="handleCameraChange('camera_autoexposure_level')"
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
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_color_bar'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_color_bar"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_color_bar']"
                    :title="propsIsOk['camera_color_bar'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_color_bar')"
                  >
                    <el-option label="关闭" :value="0"> </el-option>
                    <el-option label="开启" :value="1"> </el-option>
                  </el-select>
                </li>
                <li>
                  <i class="iconfont icon-quanping_huaban1"></i>尺寸：<el-select
                    :class="
                      'select-item ' +
                        (propsIsOk['camera_size'] ? '' : 'invalid')
                    "
                    v-model="curEchartsData.camera_size"
                    placeholder="请选择"
                    popper-class="option-list"
                    :disabled="!propsIsOk['camera_size']"
                    :title="propsIsOk['camera_size'] ? '' : '生效中'"
                    @change="handleCameraChange('camera_size')"
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
          </div>
        </div>
        <div class="rs-item">
          <i></i>
          <div
            class="stop-history-update"
            :title="stopHistoryUpdate ? '开启更新' : '停止更新'"
            @click="() => (stopHistoryUpdate = !stopHistoryUpdate)"
          >
            <i
              :class="
                `iconfont ${stopHistoryUpdate ? 'icon-sync' : 'icon-tingzhi1'}`
              "
              :style="`color:${stopHistoryUpdate ? 'yellow' : 'red'}`"
            ></i>
          </div>
          <div id="history-data"></div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    const echartsData = {
      // 速度
      cur_speed_whole: 0,
      cur_speed_angle: 0,
      set_speed_whole: 0,
      set_speed_angle: 0,
      set_speed_dir: 0,
      set_dir_speed_whole: 20,
      // 左右前后轮速度
      cur_speed_lq: 0,
      cur_speed_rq: 0,
      cur_speed_lh: 0,
      cur_speed_rh: 0,
      // 舵机
      cur_duoji_1_angle: 0,
      cur_duoji_2_angle: 0,
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
      camera_is_abort: 1, // 是否中断标志位
      // 其他标志
      camera_frame: 0, // 帧率
      // 历史数据
      history_frame: [0],
      history_speed: [0],
      history_count: [1],
    }
    return {
      echartsData,
      options: [
        // 整体速度
        {
          backgroundColor: 'transparent',
          title: {
            show: true,
            text: '速度',
            left: 'center',
            top: '5%',
            color: '#000000',
            fontSize: 14,
            width: 50,
            height: 20,
          },
          series: [
            {
              type: 'gauge',
              min: 0,
              max: 200,
              startAngle: 200,
              endAngle: -20,
              center: ['50%', '60%'],
              animationDurationUpdate: 1000,
              itemStyle: {
                color: '#77ddff',
                shadowColor: 'rgba(0,138,255,0.45)',
                shadowBlur: 10,
                shadowOffsetX: 2,
                shadowOffsetY: 2,
              },
              progress: {
                show: true,
                width: 10,
                roundCap: false,
                itemStyle: {
                  color: {
                    type: 'linear',
                    x: 0,
                    y: 0,
                    x2: 1,
                    y2: 1,
                    colorStops: [
                      {
                        offset: 0,
                        color: '#aaf', // 0% 处的颜色
                      },
                      {
                        offset: 1,
                        color: '#55f', // 100% 处的颜色
                      },
                    ],
                    global: false, // 缺省为 false
                  },
                },
              },
              axisLine: {
                lineStyle: {
                  width: 10,
                },
              },
              axisTick: {
                show: true,
              },
              splitLine: {
                length: 10,
                lineStyle: {
                  width: 2,
                  color: '#aaf',
                },
              },
              axisLabel: {
                distance: 20,
                color: '#999',
                fontSize: 9,
              },
              pointer: {
                icon:
                  'path://M2090.36389,615.30999 L2090.36389,615.30999 C2091.48372,615.30999 2092.40383,616.194028 2092.44859,617.312956 L2096.90698,728.755929 C2097.05155,732.369577 2094.2393,735.416212 2090.62566,735.56078 C2090.53845,735.564269 2090.45117,735.566014 2090.36389,735.566014 L2090.36389,735.566014 C2086.74736,735.566014 2083.81557,732.63423 2083.81557,729.017692 C2083.81557,728.930412 2083.81732,728.84314 2083.82081,728.755929 L2088.2792,617.312956 C2088.32396,616.194028 2089.24407,615.30999 2090.36389,615.30999 Z',
                length: '80%',
                width: 10,
                offsetCenter: [0, '5%'],
              },
              anchor: {
                show: true,
                showAbove: true,
                size: 15,
                itemStyle: {
                  borderWidth: 8,
                },
              },
              detail: {
                valueAnimation: true,
                fontSize: 20,
                color: '#bbbbff',
                offsetCenter: ['20%', '60%'],
                formatter: function(value) {
                  return '{value|' + value.toFixed(0) + '}{unit|cm/s}'
                },
                rich: {
                  value: {
                    fontSize: 30,
                    fontWeight: 'bolder',
                    color: '#eee',
                  },
                  unit: {
                    fontSize: 14,
                    color: '#aaa',
                    padding: [0, 0, -10, 10],
                  },
                },
              },
              data: [
                {
                  value: echartsData.cur_speed_whole,
                },
              ],
            },
          ],
          media: [
            {
              query: { maxWidth: 340 },
              option: {
                title: {
                  show: false,
                  text: '速度',
                  left: 'center',
                  top: '5%',
                  color: '#000000',
                  fontSize: 14,
                  width: 50,
                  height: 20,
                },
                series: [
                  {
                    type: 'gauge',
                    min: 0,
                    max: 200,
                    startAngle: 200,
                    endAngle: -20,
                    center: ['50%', '60%'],
                    animationDurationUpdate: 1000,
                    itemStyle: {
                      color: '#77ddff',
                      shadowColor: 'rgba(0,138,255,0.45)',
                      shadowBlur: 10,
                      shadowOffsetX: 2,
                      shadowOffsetY: 2,
                    },
                    progress: {
                      show: true,
                      width: 8,
                      roundCap: false,
                      itemStyle: {
                        color: {
                          type: 'linear',
                          x: 0,
                          y: 0,
                          x2: 1,
                          y2: 1,
                          colorStops: [
                            {
                              offset: 0,
                              color: '#aaf', // 0% 处的颜色
                            },
                            {
                              offset: 1,
                              color: '#55f', // 100% 处的颜色
                            },
                          ],
                          global: false, // 缺省为 false
                        },
                      },
                    },
                    axisLine: {
                      lineStyle: {
                        width: 8,
                      },
                    },
                    axisTick: {
                      show: true,
                    },
                    splitLine: {
                      length: 8,
                      lineStyle: {
                        width: 1,
                        color: '#aaf',
                      },
                    },
                    axisLabel: {
                      distance: 10,
                      color: '#999',
                      fontSize: 5,
                    },
                    pointer: {
                      icon:
                        'path://M2090.36389,615.30999 L2090.36389,615.30999 C2091.48372,615.30999 2092.40383,616.194028 2092.44859,617.312956 L2096.90698,728.755929 C2097.05155,732.369577 2094.2393,735.416212 2090.62566,735.56078 C2090.53845,735.564269 2090.45117,735.566014 2090.36389,735.566014 L2090.36389,735.566014 C2086.74736,735.566014 2083.81557,732.63423 2083.81557,729.017692 C2083.81557,728.930412 2083.81732,728.84314 2083.82081,728.755929 L2088.2792,617.312956 C2088.32396,616.194028 2089.24407,615.30999 2090.36389,615.30999 Z',
                      length: '80%',
                      width: 5,
                      offsetCenter: [0, '5%'],
                    },
                    anchor: {
                      show: true,
                      showAbove: true,
                      size: 10,
                      itemStyle: {
                        borderWidth: 4,
                      },
                    },
                    detail: {
                      valueAnimation: true,
                      fontSize: 10,
                      color: '#bbbbff',
                      offsetCenter: ['20%', '60%'],
                      formatter: function(value) {
                        return '{value|' + value.toFixed(0) + '}{unit|cm/s}'
                      },
                      rich: {
                        value: {
                          fontSize: 15,
                          fontWeight: 'bolder',
                          color: '#eee',
                        },
                        unit: {
                          fontSize: 8,
                          color: '#aaa',
                          padding: [0, 0, -10, 10],
                        },
                      },
                    },
                    data: [
                      {
                        value: echartsData.cur_speed_whole,
                      },
                    ],
                  },
                ],
              },
            },
          ],
        },
        // 各轮速度
        {
          backgroundColor: 'transparent',
          series: [
            {
              name: '左前轮',
              type: 'gauge',
              color: ['#f00'],
              min: -180,
              max: 180,
              splitNumber: 6,
              radius: '40%',
              animationDurationUpdate: 1000,
              center: ['28%', '28%'],
              axisLine: {
                // 坐标轴线
                lineStyle: {
                  // 属性lineStyle控制线条样式
                  width: 5,
                  color: [
                    [0.5, '#ee5511'],
                    [1, '#1155ee'],
                  ],
                  shadowColor: '#7f7f7f', //默认透明
                  shadowBlur: 2,
                },
                backgroundColor: 'none',
              },
              axisTick: {
                // 坐标轴小标记
                length: 5, // 属性length控制线长
                lineStyle: {
                  // 属性lineStyle控制线条样式
                  color: 'auto',
                },
                distance: 1,
              },
              splitLine: {
                // 分隔线
                length: 3, // 属性length控制线长
                lineStyle: {
                  // 属性lineStyle（详见lineStyle）控制线条样式
                  color: 'rgba(255,255,255,0.7)',
                },
                distance: 1,
              },
              axisLabel: {
                borderRadius: 1,
                color: 'rgba(255,255,255,0.7)',
                padding: 1,
                distance: 10,
                fontSize: 8,
              },
              title: {
                // 其余属性默认使用全局文本样式，详见TEXTSTYLE
                // fontWeight: 'bolder',
                fontSize: 10,
                fontColor: '#FFF',
                color: '#FFF',
                paddingTop: 5,
                offsetCenter: [0, '95%'],
                // fontStyle: 'italic'
              },
              itemStyle: {
                color: '#1092ff',
              },
              detail: {
                shadowOffsetX: 0,
                shadowOffsetY: 0,
                // borderWidth: 1,
                textBorderColor: '#000',
                textBorderWidth: 1,
                textShadowBlur: 1,
                textShadowColor: '#fff',
                textShadowOffsetX: 0,
                textShadowOffsetY: 0,
                paddingTop: 5,
                fontFamily: 'digital',
                fontSize: 12,
                width: 15,
                color: '#fff',
                rich: {},
                offsetCenter: [10, '70%'],
                formatter: function(value) {
                  return '{value|' + value.toFixed(0) + '}{unit|cm/s}'
                },
                rich: {
                  value: {
                    fontSize: 12,
                    fontWeight: 'bolder',
                    color: '#eee',
                  },
                  unit: {
                    fontSize: 8,
                    color: '#eee',
                    padding: [0, 0, -2, 5],
                  },
                },
              },
              data: [
                {
                  value: echartsData.cur_speed_lq,
                  name: '左前轮',
                },
              ],
            },
            {
              name: '右前轮',
              type: 'gauge',
              color: ['#f00'],
              min: -180,
              max: 180,
              splitNumber: 6,
              radius: '40%',
              animationDurationUpdate: 1000,
              center: ['72%', '28%'],
              axisLine: {
                // 坐标轴线
                lineStyle: {
                  // 属性lineStyle控制线条样式
                  width: 5,
                  color: [
                    [0.5, '#ee5511'],
                    [1, '#1155ee'],
                  ],
                  shadowColor: '#7f7f7f', //默认透明
                  shadowBlur: 2,
                },
                backgroundColor: 'none',
              },
              axisTick: {
                // 坐标轴小标记
                length: 5, // 属性length控制线长
                lineStyle: {
                  // 属性lineStyle控制线条样式
                  color: 'auto',
                },
                distance: 1,
              },
              splitLine: {
                // 分隔线
                length: 3, // 属性length控制线长
                lineStyle: {
                  // 属性lineStyle（详见lineStyle）控制线条样式
                  color: 'rgba(255,255,255,0.7)',
                },
                distance: 1,
              },
              axisLabel: {
                borderRadius: 1,
                color: 'rgba(255,255,255,0.7)',
                padding: 1,
                distance: 10,
                fontSize: 8,
              },
              title: {
                // 其余属性默认使用全局文本样式，详见TEXTSTYLE
                // fontWeight: 'bolder',
                fontSize: 10,
                fontColor: '#FFF',
                color: '#FFF',
                paddingTop: 5,
                offsetCenter: [0, '95%'],
                // fontStyle: 'italic'
              },
              itemStyle: {
                color: '#1092ff',
              },
              detail: {
                shadowOffsetX: 0,
                shadowOffsetY: 0,
                // borderWidth: 1,
                textBorderColor: '#000',
                textBorderWidth: 1,
                textShadowBlur: 1,
                textShadowColor: '#fff',
                textShadowOffsetX: 0,
                textShadowOffsetY: 0,
                paddingTop: 5,
                fontFamily: 'digital',
                fontSize: 12,
                width: 15,
                color: '#fff',
                rich: {},
                offsetCenter: [10, '70%'],
                formatter: function(value) {
                  return '{value|' + value.toFixed(0) + '}{unit|cm/s}'
                },
                rich: {
                  value: {
                    fontSize: 12,
                    fontWeight: 'bolder',
                    color: '#eee',
                  },
                  unit: {
                    fontSize: 8,
                    color: '#eee',
                    padding: [0, 0, -2, 5],
                  },
                },
              },
              data: [
                {
                  value: echartsData.cur_speed_rq,
                  name: '右前轮',
                },
              ],
            },
            {
              name: '左后轮',
              type: 'gauge',
              color: ['#f00'],
              min: -180,
              max: 180,
              splitNumber: 6,
              radius: '40%',
              animationDurationUpdate: 1000,
              center: ['28%', '75%'],
              axisLine: {
                // 坐标轴线
                lineStyle: {
                  // 属性lineStyle控制线条样式
                  width: 5,
                  color: [
                    [0.5, '#ee5511'],
                    [1, '#1155ee'],
                  ],
                  shadowColor: '#7f7f7f', //默认透明
                  shadowBlur: 2,
                },
                backgroundColor: 'none',
              },
              axisTick: {
                // 坐标轴小标记
                length: 5, // 属性length控制线长
                lineStyle: {
                  // 属性lineStyle控制线条样式
                  color: 'auto',
                },
                distance: 1,
              },
              splitLine: {
                // 分隔线
                length: 3, // 属性length控制线长
                lineStyle: {
                  // 属性lineStyle（详见lineStyle）控制线条样式
                  color: 'rgba(255,255,255,0.7)',
                },
                distance: 1,
              },
              axisLabel: {
                borderRadius: 1,
                color: 'rgba(255,255,255,0.7)',
                padding: 1,
                distance: 10,
                fontSize: 8,
              },
              title: {
                // 其余属性默认使用全局文本样式，详见TEXTSTYLE
                // fontWeight: 'bolder',
                fontSize: 10,
                fontColor: '#FFF',
                color: '#FFF',
                paddingTop: 5,
                offsetCenter: [0, '95%'],
                // fontStyle: 'italic'
              },
              itemStyle: {
                color: '#1092ff',
              },
              detail: {
                shadowOffsetX: 0,
                shadowOffsetY: 0,
                // borderWidth: 1,
                textBorderColor: '#000',
                textBorderWidth: 1,
                textShadowBlur: 1,
                textShadowColor: '#fff',
                textShadowOffsetX: 0,
                textShadowOffsetY: 0,
                paddingTop: 5,
                fontFamily: 'digital',
                fontSize: 12,
                width: 15,
                color: '#fff',
                rich: {},
                offsetCenter: [10, '70%'],
                formatter: function(value) {
                  return '{value|' + value.toFixed(0) + '}{unit|cm/s}'
                },
                rich: {
                  value: {
                    fontSize: 12,
                    fontWeight: 'bolder',
                    color: '#eee',
                  },
                  unit: {
                    fontSize: 8,
                    color: '#eee',
                    padding: [0, 0, -2, 5],
                  },
                },
              },
              data: [
                {
                  value: echartsData.cur_speed_lh,
                  name: '左后轮',
                },
              ],
            },
            {
              name: '右后轮',
              type: 'gauge',
              color: ['#f00'],
              min: -180,
              max: 180,
              splitNumber: 6,
              radius: '40%',
              animationDurationUpdate: 1000,
              center: ['72%', '75%'],
              axisLine: {
                // 坐标轴线
                lineStyle: {
                  // 属性lineStyle控制线条样式
                  width: 5,
                  color: [
                    [0.5, '#ee5511'],
                    [1, '#1155ee'],
                  ],
                  shadowColor: '#7f7f7f', //默认透明
                  shadowBlur: 2,
                },
                backgroundColor: 'none',
              },
              axisTick: {
                // 坐标轴小标记
                length: 5, // 属性length控制线长
                lineStyle: {
                  // 属性lineStyle控制线条样式
                  color: 'auto',
                },
                distance: 1,
              },
              splitLine: {
                // 分隔线
                length: 3, // 属性length控制线长
                lineStyle: {
                  // 属性lineStyle（详见lineStyle）控制线条样式
                  color: 'rgba(255,255,255,0.7)',
                },
                distance: 1,
              },
              axisLabel: {
                borderRadius: 1,
                color: 'rgba(255,255,255,0.7)',
                padding: 1,
                distance: 10,
                fontSize: 8,
              },
              title: {
                // 其余属性默认使用全局文本样式，详见TEXTSTYLE
                // fontWeight: 'bolder',
                fontSize: 10,
                fontColor: '#FFF',
                color: '#FFF',
                paddingTop: 5,
                offsetCenter: [0, '95%'],
                // fontStyle: 'italic'
              },
              itemStyle: {
                color: '#1092ff',
              },
              detail: {
                shadowOffsetX: 0,
                shadowOffsetY: 0,
                // borderWidth: 1,
                textBorderColor: '#000',
                textBorderWidth: 1,
                textShadowBlur: 1,
                textShadowColor: '#fff',
                textShadowOffsetX: 0,
                textShadowOffsetY: 0,
                paddingTop: 5,
                fontFamily: 'digital',
                fontSize: 12,
                width: 15,
                color: '#fff',
                rich: {},
                offsetCenter: [10, '70%'],
                formatter: function(value) {
                  return '{value|' + value.toFixed(0) + '}{unit|cm/s}'
                },
                rich: {
                  value: {
                    fontSize: 12,
                    fontWeight: 'bolder',
                    color: '#eee',
                  },
                  unit: {
                    fontSize: 8,
                    color: '#eee',
                    padding: [0, 0, -2, 5],
                  },
                },
              },
              data: [
                {
                  value: echartsData.cur_speed_rh,
                  name: '右后轮',
                },
              ],
            },
          ],
        },
        // 舵机
        {
          backgroundColor: 'transparent',
          title: {
            show: true,
            text: '舵机角度',
            left: 'center',
            top: '5%',
            color: '#000000',
            fontSize: 12,
            width: 50,
            height: 20,
          },
          series: [
            // 一维舵机
            // 内侧刻度
            {
              type: 'gauge',
              radius: '50%', // 位置
              center: ['30%', '55%'],
              min: 135,
              max: -135,
              startAngle: 225,
              endAngle: -45,
              axisLine: {
                show: true,
                lineStyle: {
                  // 轴线样式
                  width: 40, // 宽度
                  color: [
                    [
                      1,
                      new this.$echarts.graphic.LinearGradient(0, 0, 0, 1, [
                        {
                          offset: 0,
                          color: '#5098ed88',
                        },
                        {
                          offset: 1,
                          color: '#5098ed08',
                        },
                      ]),
                    ],
                  ], // 颜色
                },
              },
              axisTick: {
                // 刻度
                show: true,
                splitNumber: 2,
                length: 5,
                distance: -45,
              },
              splitLine: {
                // 分割线
                show: false,
              },
              axisLabel: {
                // 刻度标签
                show: true,
                fontSize: 6,
                distance: -35,
              },
              pointer: {
                // 仪表盘指针
                show: false,
              },
              detail: {
                // 仪表盘详情
                show: false,
              },
            },
            // 内侧轴线
            {
              type: 'gauge',
              radius: '50%', // 位置
              center: ['30%', '55%'],
              min: 135,
              max: -135,
              startAngle: 225,
              endAngle: -45,
              axisLine: {
                show: true,
                lineStyle: {
                  // 轴线样式
                  width: 3, // 宽度
                  color: [
                    [0, '#00FCF7'],
                    [1, '#5098cc'],
                  ], // 颜色
                },
              },
              pointer: {
                // 仪表盘指针
                show: false,
              },
              axisTick: {
                // 刻度
                show: false,
              },
              splitLine: {
                // 分割线
                show: false,
              },
              axisLabel: {
                // 刻度标签
                show: false,
              },
              detail: {
                // 仪表盘详情
                show: false,
              },
            },
            // 指针
            {
              type: 'gauge',
              animationDurationUpdate: 1000,
              radius: '20%', // 位置
              center: ['30%', '55%'],
              min: 135,
              max: -135,
              startAngle: 225,
              endAngle: -45,
              axisLine: {
                show: false,
              },
              pointer: {
                // 仪表盘指针
                show: true,
                icon: 'roundRect',
                length: '220%',
                width: '10',
                offsetCenter: [0, 0],
              },
              anchor: {
                show: true,
                showAbove: true,
                size: 10,
                itemStyle: {
                  color: '#5098aa',
                },
                icon: 'circle',
              },
              itemStyle: {
                color: 'rgba(0,255,255,0.3)',
              },
              axisTick: {
                // 刻度
                show: false,
              },
              splitLine: {
                // 分割线
                show: false,
              },
              axisLabel: {
                // 刻度标签
                show: false,
              },
              detail: {
                // 仪表盘详情
                show: true,
                offsetCenter: ['20%', '140%'],
                fontSize: '8',
                color: '#08b5d6',
                formatter: function(value) {
                  return '{value|' + value.toFixed(0) + '}{unit|°}'
                },
                rich: {
                  value: {
                    fontSize: 12,
                    fontWeight: 'bolder',
                    color: '#eee',
                  },
                  unit: {
                    fontSize: 10,
                    color: '#eee',
                    padding: [0, 0, 2, 3],
                  },
                },
              },
              data: [
                {
                  value: echartsData.cur_duoji_1_angle,
                  name: '一维舵机',
                },
              ],
              title: {
                fontSize: 10,
                fontColor: '#FFF',
                color: '#FFF',
                paddingTop: 5,
                offsetCenter: [0, '220%'],
              },
            },
            // 二维舵机
            // 内侧刻度
            {
              type: 'gauge',
              radius: '50%', // 位置
              center: ['75%', '55%'],
              min: 45,
              max: -135,
              startAngle: 225,
              endAngle: 45,
              axisLine: {
                show: true,
                lineStyle: {
                  // 轴线样式
                  width: 40, // 宽度
                  color: [
                    [
                      1,
                      new this.$echarts.graphic.LinearGradient(0, 0, 0, 1, [
                        {
                          offset: 0,
                          color: '#5098ed38',
                        },
                        {
                          offset: 0.5,
                          color: '#5098ed88',
                        },
                        {
                          offset: 1,
                          color: '#5098ed38',
                        },
                      ]),
                    ],
                  ], // 颜色
                },
              },
              axisTick: {
                // 刻度
                show: true,
                splitNumber: 2,
                length: 5,
                distance: -45,
              },
              splitLine: {
                // 分割线
                show: false,
              },
              axisLabel: {
                // 刻度标签
                show: true,
                fontSize: 6,
                distance: -35,
              },
              pointer: {
                // 仪表盘指针
                show: false,
              },
              detail: {
                // 仪表盘详情
                show: false,
              },
            },
            // 内侧轴线
            {
              type: 'gauge',
              radius: '50%', // 位置
              center: ['75%', '55%'],
              min: 45,
              max: -135,
              startAngle: 225,
              endAngle: 45,
              axisLine: {
                show: true,
                lineStyle: {
                  // 轴线样式
                  width: 3, // 宽度
                  color: [
                    [0, '#00FCF7'],
                    [1, '#5098cc'],
                  ], // 颜色
                },
              },
              pointer: {
                // 仪表盘指针
                show: false,
              },
              axisTick: {
                // 刻度
                show: false,
              },
              splitLine: {
                // 分割线
                show: false,
              },
              axisLabel: {
                // 刻度标签
                show: false,
              },
              detail: {
                // 仪表盘详情
                show: false,
              },
            },
            // 指针
            {
              type: 'gauge',
              animationDurationUpdate: 1000,
              radius: '20%', // 位置
              center: ['75%', '55%'],
              min: 45,
              max: -135,
              startAngle: 225,
              endAngle: 45,
              axisLine: {
                show: false,
              },
              pointer: {
                // 仪表盘指针
                show: true,
                icon: 'roundRect',
                length: '220%',
                width: '10',
                offsetCenter: [0, 0],
              },
              anchor: {
                show: true,
                showAbove: true,
                size: 10,
                itemStyle: {
                  color: '#5098aa',
                },
                icon: 'circle',
              },
              itemStyle: {
                color: 'rgba(0,255,255,0.3)',
              },
              axisTick: {
                // 刻度
                show: false,
              },
              splitLine: {
                // 分割线
                show: false,
              },
              axisLabel: {
                // 刻度标签
                show: false,
              },
              detail: {
                // 仪表盘详情
                show: true,
                offsetCenter: ['140%', '0%'],
                fontSize: '8',
                color: '#08b5d6',
                formatter: function(value) {
                  return '{value|' + value.toFixed(0) + '}{unit|°}'
                },
                rich: {
                  value: {
                    fontSize: 12,
                    fontWeight: 'bolder',
                    color: '#eee',
                  },
                  unit: {
                    fontSize: 10,
                    color: '#eee',
                    padding: [0, 0, 2, 3],
                  },
                },
              },
              data: [
                {
                  value: echartsData.cur_duoji_2_angle,
                  name: '二维舵机',
                },
              ],
              title: {
                fontSize: 10,
                fontColor: '#FFF',
                color: '#FFF',
                paddingTop: 5,
                offsetCenter: ['140%', '80%'],
              },
            },
          ],
        },
        // 历史数据折线图
        {
          backgroundColor: 'transparent',
          tooltip: {
            trigger: 'axis',
            axisPointer: {
              type: 'shadow',
              textStyle: {
                color: '#fff',
              },
            },
          },
          grid: {
            borderWidth: 0,
            top: 30,
            bottom: 50,
            textStyle: {
              color: '#fff',
            },
          },
          legend: {
            x: '50%',
            top: '5%',
            textStyle: {
              color: '#90979c',
            },
            data: ['帧率', '速度'],
          },
          calculable: true,
          xAxis: [
            {
              type: 'category',
              axisLine: {
                lineStyle: {
                  color: 'rgba(204,187,225,0.5)',
                },
              },
              splitLine: {
                show: false,
              },
              axisTick: {
                show: false,
              },
              data: [
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                10,
                11,
                12,
                13,
                14,
                15,
                16,
                17,
                18,
                19,
                20,
                21,
                22,
                23,
                24,
                25,
                26,
                27,
                28,
                29,
                30,
              ], //echartsData.history_count,
            },
          ],
          yAxis: [
            {
              type: 'value',
              splitLine: {
                show: false,
              },
              axisLine: {
                lineStyle: {
                  color: 'rgba(204,187,225,0.5)',
                },
              },
            },
          ],
          dataZoom: [
            {
              show: true,
              height: 20,
              xAxisIndex: [0],
              bottom: 10,
              start: 70,
              end: 100,
              handleIcon:
                'path://M306.1,413c0,2.2-1.8,4-4,4h-59.8c-2.2,0-4-1.8-4-4V200.8c0-2.2,1.8-4,4-4h59.8c2.2,0,4,1.8,4,4V413z',
              handleSize: '110%',
              handleStyle: {
                color: '#5B3AAE',
              },
              textStyle: {
                color: 'rgba(204,187,225,0.5)',
              },
              fillerColor: 'rgba(67,55,160,0.4)',
              borderColor: 'rgba(204,187,225,0.5)',
            },
            {
              type: 'inside',
              show: true,
              height: 20,
              start: 0,
              end: 100,
            },
          ],
          series: [
            {
              name: '帧率',
              type: 'line',
              symbolSize: 6,
              symbol: 'circle',
              itemStyle: {
                color: '#6f7de3',
              },
              data: echartsData.history_frame,
            },
            {
              name: '速度',
              type: 'line',
              symbolSize: 6,
              symbol: 'circle',
              itemStyle: {
                color: '#c257F6',
              },
              data: echartsData.history_speed,
            },
          ],
        },
      ],
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
      curEchartsData: {
        // 速度
        cur_speed_whole: 0,
        cur_speed_angle: 0,
        set_speed_whole: 0,
        set_speed_angle: 0,
        set_speed_dir: 0,
        set_dir_speed_whole: 20,
        // 左右前后轮速度
        cur_speed_lq: 0,
        cur_speed_rq: 0,
        cur_speed_lh: 0,
        cur_speed_rh: 0,
        // 舵机
        cur_duoji_1_angle: 0,
        cur_duoji_2_angle: 0,
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
        camera_is_abort: 1, // 是否中断标志位
        // 其他标志
        camera_frame: 0, // 帧率
        // 历史数据
        history_frame: [0],
        history_speed: [0],
        history_count: [1],
      },
      els: [],
      myEcharts: [],
      wss: null,
      dragEl: null,
      gunEl: null,
      maxDistance: 80,
      statesData: null,
      DeviceName: 'test2',
      ProductId: 'K8LG8U17CW',
      clientToken: '',
      propsIsOk: {},
      propsTime: {},
      selectList: [0, 1, 2, 3, 4],
      stopHistoryUpdate: false,
      preImgData: null,
      // 需要云端同步到设备的属性
      syncProps: [
        'set_speed_whole',
        'set_speed_dir',
        'set_dir_speed_whole',
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
        'camera_is_abort',
      ],
      hasReceiveImgData: false,
      mqttIsConnected: false,
      mqttIsConnectedTime: 0,
      controlIsConnected: false, // 小车控制连接状态
      controlIsConnectedTime: 0,
      speedLevel: 8,
      carSelectList: [],
      curSelectCar: '',
      historyTimer: 0,
      isActive: false,
    }
  },
  watch: {
    echartsData: {
      handler: 'putEchartsData',
      deep: true,
    },
    'curEchartsData.camera_is_abort'() {
      this.hasReceiveImgData = false
    },
  },
  mounted() {
    this.mapElementToEcharts()
    this.drawAllEcharts()
    this.openWebSocket()
    this.initDrag()

    // 摇杆节流
    this.setMove = this.throttle(this.setMove, 200)
  },
  activated() {
    this.isActive = true
    this.statesData = null
    this.initShowHistory()
    // 获取需同步的数据
    this.syncData()
    // 获取默认配置，初始化同步数据
    this.syncDefaultConfig()
    // 获取用户名下设备列表
    this.getDeviceByUser()
  },
  deactivated() {
    this.isActive = false
    clearInterval(this.historyTimer)
    clearTimeout(this.mqttIsConnectedTime)
    clearTimeout(this.controlIsConnectedTime)
  },
  beforeDestroy() {
    // 关闭ws连接
    this.wss && this.wss.close()
    this.wss = null
    this.els = null
    this.myEcharts = null
    this.dragEl = null
    this.gunEl = null
    window.onresize = null
  },
  computed: {
    // 摇杆速度系数
    K() {
      return 160 / this.speedLevel / this.maxDistance
    },
  },
  methods: {
    /* 请求 */
    // 通用修改属性
    putProps(url, params) {
      const user = localStorage.getItem('iotc_user') || ''
      const secret = localStorage.getItem('iotc_secret') || ''
      return this.$http.put(url, {
        Payload: JSON.stringify({
          clientToken: this.clientToken,
          method: 'control',
          params: params,
        }),
        DeviceName: this.DeviceName,
        ProductId: this.ProductId,
        Topic: `$thing/down/property/${this.ProductId}/${this.DeviceName}`,
        user,
        secret,
      })
    },
    // 获取历史属性值
    getHistoryProp(FieldName, Limit = 30) {
      const user = localStorage.getItem('iotc_user') || ''
      const secret = localStorage.getItem('iotc_secret') || ''
      return this.$http.post(`/historyPropValue`, {
        DeviceName: this.DeviceName,
        FieldName: FieldName,
        Limit: Limit,
        user,
        secret,
      })
    },
    // 获取需同步的数据
    getSyncData() {
      const user = localStorage.getItem('iotc_user') || ''
      const secret = localStorage.getItem('iotc_secret') || ''
      return this.$http.post(`/get/sync`, {
        user,
        secret,
      })
    },
    // 获取默认配置
    getDefaultConfig() {
      const user = localStorage.getItem('iotc_user') || ''
      const secret = localStorage.getItem('iotc_secret') || ''
      return this.$http.post(`/get/default`, {
        user,
        secret,
      })
    },
    // 获取设备列表
    getDeviceList() {
      const user = localStorage.getItem('iotc_user') || ''
      const secret = localStorage.getItem('iotc_secret') || ''
      return this.$http.post(`/public/deviceList`, {
        user,
        secret,
      })
    },
    // 小车移动
    putMove(params, debounce = false, time = 0) {
      // 处于节流中，直接不响应指令
      if (this.putMove.timer) return
      const user = localStorage.getItem('iotc_user') || ''
      const secret = localStorage.getItem('iotc_secret') || ''
      // 配置节流
      if (debounce) {
        this.putMove.timer = setTimeout(() => {
          this.putMove.timer = 0
        }, time)
      }
      // 指令映射表
      // set_speed_whole:w-0
      // set_speed_angle:a-0
      // set_speed_dir:d-0
      // set_dir_speed_whole:w-1
      // wifi2_reset: r-1
      // system_reset: r-2
      // receive_app: r-3
      // entry_app: a-1
      // cancel_auto_app: c-0

      const table = {
        set_speed_whole: 'w-0',
        set_speed_angle: 'a-0',
        set_speed_dir: 'd-0',
        set_dir_speed_whole: 'w-1',
      }
      const data = {}
      // 序列化指令
      for (const key in params) {
        data[table[key]] = params[key]
      }

      return this.$http.post(`/move`, { user, secret, data, debounce, time })
    },
    // 获取历史数据
    async showHistoryData(field, key) {
      try {
        var res = await this.getHistoryProp(field)
      } catch (error) {}
      if (res && res.data && res.data.Results instanceof Array) {
        this.echartsData[key] = res.data.Results.map((data) => +data.Value)
      }
    },
    // 修改属性
    async propsChange(type, isNoTime = false) {
      // 指令下发超时配置
      if (!isNoTime) {
        this.propsIsOk[type] = false
        this.propsTime[type] = setTimeout(() => {
          this.propsIsOk[type] = true
          this.curEchartsData[type] = this.echartsData[type]
        }, 8000)
      }
      try {
        const params = { [type]: this.curEchartsData[type] }
        var res = await this.putProps(`/putProps`, params)
      } catch (error) {}
      // console.log(res)
    },
    // 更新默认配置
    async syncDefaultConfig() {
      try {
        var res = await this.getDefaultConfig()
      } catch (error) {}
      if (res.data && res.data.data) {
        const fields = JSON.parse(res.data.data)
        Object.assign(this.curEchartsData, fields)
      }
    },
    // 获取需同步的数据
    async syncData() {
      try {
        var res = await this.getSyncData()
      } catch (error) {}
      if (res.data.data) {
        const syncDatas = res.data.data.split(',')
        this.syncProps = syncDatas
        // console.log(syncDatas)
      }
    },
    // 获取用户所属设备
    async getDeviceByUser() {
      try {
        var res = await this.getDeviceList()
      } catch (error) {}
      if (res.data && res.data.data) {
        this.carSelectList = JSON.parse(res.data.data) || []
        const secret = localStorage.getItem('iotc_secret')
        if (secret) {
          this.carSelectList.forEach((car) => {
            if (car.secret === secret) {
              this.curSelectCar = car.name
            }
          })
        }
      }
      this.showMessage(res.data.state, res.data.message)
    },
    /* 事件 */
    // 切换连接
    handleChangeCar(secret) {
      localStorage.setItem('iotc_secret', secret)
    },
    // 移动端开关
    handleChangeCameraState(state) {
      this.curEchartsData.camera_is_abort = state
      this.handleCameraChange('camera_is_abort')
    },
    // 设置速度挡位
    handleSpeedLevel(level) {
      this.speedLevel = level
    },
    // 同步数据
    handleSyncData(type) {
      // 小车数据同步到云端
      if (type === 'car') {
        // 获取过设备数据，才进行更新
        if (this.statesData) {
          // 只下发需同步的数据
          for (const key of this.syncProps) {
            this.curEchartsData[key] = this.echartsData[key]
          }
        }
      } else {
        // 云端数据同步到小车
        this.hasReceiveImgData = false
        // 只下发需同步的数据
        for (const key of this.syncProps) {
          if (key === 'history_frame' || key === 'history_speed') contniue
          this.propsChange(key)
        }
      }
    },
    // 保存图片
    handleSaveImg() {
      const link = document.createElement('a')
      link.style.display = 'none'
      link.href = this.preImgData
      link.setAttribute('download', `图片${Date.now()}.jpg`)
      document.body.appendChild(link)
      link.click()
    },
    // 小车旋转
    handleDirChange(dir) {
      this.curEchartsData.set_speed_dir = dir
      if (dir) {
        // 旋转
        this.putMove({
          set_dir_speed_whole: this.curEchartsData.set_dir_speed_whole,
          set_speed_dir: dir,
        })
      } else {
        // 停止旋转，即归位
        this.resetMove()
      }
    },
    // 摄像头设置
    handleCameraChange(type) {
      this.propsChange(type)
    },
    // 舵机复位
    handleDuojiReset() {
      this.curEchartsData.cur_duoji_1_angle = 0
      this.curEchartsData.cur_duoji_2_angle = 0
      this.propsChange('cur_duoji_1_angle')
      this.propsChange('cur_duoji_2_angle')
    },
    // 舵机调整
    handleChangeDuoji(type) {
      this.propsChange(type)
    },
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
    // 重置元素位置
    setEl(startLeft, startTop, distance = 0) {
      this.dragEl.display = 'none'
      this.gunEl.display = 'none'
      this.dragEl.style.left = startLeft + 'px'
      this.dragEl.style.top = startTop + 'px'
      this.gunEl.style.width = distance + 'px'
      this.dragEl.display = 'block'
      this.gunEl.display = 'block'
    },
    // 重置移动
    resetMove() {
      // 发送停止指令，即归位
      this.putMove(
        {
          set_speed_whole: 0,
          set_speed_angle: 0,
          set_speed_dir: 0,
          set_dir_speed_whole: this.curEchartsData.set_dir_speed_whole,
        },
        true,
        500
      )
    },
    // 节流
    throttle(func, interval = 0, immediately = false) {
      var timeID = null
      return function() {
        if (timeID) return Promise.resolve()
        var thisArg = this
        if (immediately) {
          immediately = false
          return new Promise((resolve, reject) => {
            timeID = setTimeout(() => {
              resolve(func.call(thisArg, ...arguments))
              timeID = null
            })
          })
        } else {
          return new Promise((resolve, reject) => {
            timeID = setTimeout(() => {
              resolve(func.call(thisArg, ...arguments))
              timeID = null
            }, interval)
          })
        }
      }
    },
    // 控制小车移动
    setMove(distance, degree, x, y) {
      distance = distance * this.K
      if (x === 0 && y === 0) {
        degree = 0
      } else if (x >= 0 && y >= 0) {
        // 第四象限
        degree = 270 - (degree * 180) / Math.PI
      } else if (x >= 0 && y <= 0) {
        // 第一象限
        degree = 270 + (degree * 180) / Math.PI
      } else if (x <= 0 && y >= 0) {
        // 第三象限
        degree = 90 + (degree * 180) / Math.PI
      } else if (x <= 0 && y <= 0) {
        // 第二象限
        degree = 90 - (degree * 180) / Math.PI
      }
      // 发送移动指令
      this.putMove({
        set_speed_whole: Math.round(distance),
        set_speed_angle: Math.round(degree),
        set_speed_dir: 0,
      })
    },
    // 轮询mqtt连接状态
    pollMqttState() {
      // 由断开切换至连接时提示
      if (!this.mqttIsConnected) {
        this.$notify({
          message: '小车已连接MQTT服务器',
          duration: 2000,
          customClass: 'notify-box',
        })
      }
      this.mqttIsConnected = true
      sessionStorage.setItem(
        'iotc_state',
        JSON.stringify({
          mqttIsConnected: true,
          controlIsConnected: this.controlIsConnected,
        })
      )

      if (this.mqttIsConnectedTime) clearTimeout(this.mqttIsConnectedTime)
      this.mqttIsConnectedTime = setTimeout(() => {
        // 超时，未收到推送数据
        // mqtt标记断开
        this.mqttIsConnected = false
        sessionStorage.setItem(
          'iotc_state',
          JSON.stringify({
            mqttIsConnected: false,
            controlIsConnected: this.controlIsConnected,
          })
        )
        // 标记未收到推送数据
        this.statesData = null
        // 标记视频断开
        this.$notify({
          message: 'MQTT服务器连接已断开',
          duration: 2000,
          customClass: 'notify-box',
        })
      }, 10000)
    },
    // 轮询云控制连接状态
    pollControlState() {
      // 由断开切换至连接时提示
      if (!this.controlIsConnected) {
        this.$notify({
          message: '小车已连接云控制服务器',
          duration: 2000,
          customClass: 'notify-box',
        })
      }
      this.controlIsConnected = true
      sessionStorage.setItem(
        'iotc_state',
        JSON.stringify({
          mqttIsConnected: this.mqttIsConnected,
          controlIsConnected: true,
        })
      )

      if (this.controlIsConnectedTime) clearTimeout(this.controlIsConnectedTime)
      this.controlIsConnectedTime = setTimeout(() => {
        // 超时，未收到推送数据
        // 标记断开
        this.controlIsConnected = false
        sessionStorage.setItem(
          'iotc_state',
          JSON.stringify({
            mqttIsConnected: this.mqttIsConnected,
            controlIsConnected: false,
          })
        )

        // 标记未接收到图像数据
        this.hasReceiveImgData = false
        // 标记视频断开
        // this.curEchartsData.camera_is_abort = 1
        this.$notify({
          message: '云控制服务器连接已断开',
          duration: 2000,
          customClass: 'notify-box',
        })
      }, 10000)
    },
    // 初始化显示历史属性值
    initShowHistory() {
      this.historyTimer = setInterval(() => {
        if (!this.stopHistoryUpdate) {
          // 基于同步数据进行同步
          if (this.syncProps.includes('history_frame')) {
            this.showHistoryData('camera_frame', 'history_frame')
          }
          if (this.syncProps.includes('history_speed')) {
            this.showHistoryData('cur_speed_whole', 'history_speed')
          }
        }
      }, 2000)
    },
    // 设置摇杆旋转角度
    setRotate(x, y, degree) {
      this.gunEl.display = 'none'
      if (x >= 0 && y >= 0) {
        this.gunEl.style.transform = `rotate(${(degree * 180) / Math.PI}deg)`
      } else if (x <= 0 && y <= 0) {
        this.gunEl.style.transform = `rotate(${((Math.PI + degree) * 180) /
          Math.PI}deg)`
      } else if (x <= 0 && y >= 0) {
        this.gunEl.style.transform = `rotate(${((Math.PI - degree) * 180) /
          Math.PI}deg)`
      } else if (x >= 0 && y <= 0) {
        this.gunEl.style.transform = `rotate(${(-degree * 180) / Math.PI}deg)`
      }
      this.gunEl.display = 'block'
    },
    // 获取摇杆旋转角度
    getDegree(x1, y1, x2, y2) {
      if (x2 === x1) return Math.PI / 2
      if (y2 === y1) return 0
      return Math.abs(Math.atan((y2 - y1) / (x2 - x1)))
    },
    // 获取摇杆长度
    getDistance(x1, y1, x2, y2) {
      const b = Math.abs(y1 - y2)
      const c = Math.abs(x1 - x2)
      return b / Math.sin(Math.atan(b / c)) || c
    },
    // 摇杆拖拽初始化
    initDrag() {
      this.dragEl = document.getElementById('drag-pointer')
      this.gunEl = document.getElementById('drag-gun')
      const startLeft = this.dragEl.offsetLeft
      const startTop = this.dragEl.offsetTop
      let startX,
        startY,
        x,
        y,
        degree,
        distance,
        isDrag = false
      startX = startY = x = y = degree = distance = 0

      // 摇杆按下
      const mousedown = (end) => {
        return (e) => {
          isDrag = true
          startX = end == 'pc' ? e.pageX : e.changedTouches[0].pageX
          startY = end == 'pc' ? e.pageY : e.changedTouches[0].pageY
        }
      }
      this.dragEl.onmousedown = mousedown('pc')
      // 移动端
      this.dragEl.ontouchstart = mousedown()

      // 摇杆弹起
      const mouseup = (e) => {
        isDrag = false
        this.setEl(startLeft, startTop)
        this.resetMove()
      }
      this.dragEl.onmouseup = mouseup
      this.dragEl.ontouchend = mouseup
      this.dragEl.parentElement.parentElement.onmouseup = mouseup
      this.dragEl.parentElement.parentElement.ontouchend = mouseup

      // 摇杆移动
      const mousemove = (end) => {
        return (e) => {
          if (isDrag) {
            const eX = end == 'pc' ? e.pageX : e.changedTouches[0].pageX
            const eY = end == 'pc' ? e.pageY : e.changedTouches[0].pageY
            degree = this.getDegree(startX, startY, eX, eY)
            distance = this.getDistance(startX, startY, eX, eY)
            if (distance > this.maxDistance) {
              distance = this.maxDistance
              x =
                Math.cos(degree) *
                (eX > startX ? this.maxDistance : -this.maxDistance)
              y =
                Math.sin(degree) *
                (eY > startY ? this.maxDistance : -this.maxDistance)
            } else {
              x = eX - startX
              y = eY - startY
            }
            this.setRotate(x, y, degree)
            this.setEl(startLeft + x, startTop + y, distance + 5)
            this.setMove(distance, degree, x, y)
          }
        }
      }
      this.dragEl.parentElement.parentElement.onmousemove = mousemove('pc')
      this.dragEl.parentElement.parentElement.ontouchmove = mousemove()
    },
    // 更新图表数据
    putEchartsData() {
      // 客户端获取过服务器推送的小车数据
      if (this.statesData) {
        const options = this.options
        const echarts = this.echartsData
        const curEcharts = this.curEchartsData
        // 更新速度相关的数据
        options[0].series[0].data[0].value = echarts.cur_speed_whole
        options[1].series[0].data[0].value = echarts.cur_speed_lq
        options[1].series[1].data[0].value = echarts.cur_speed_rq
        options[1].series[2].data[0].value = echarts.cur_speed_lh
        options[1].series[3].data[0].value = echarts.cur_speed_rh

        // 更新舵机相关数据
        options[2].series[2].data[0].value = -echarts.cur_duoji_1_angle
        options[2].series[5].data[0].value = -echarts.cur_duoji_2_angle

        // 使能获取历史数据
        if (!this.stopHistoryUpdate) {
          let arr
          // 更新历史帧率
          if (this.syncProps.includes('history_frame')) {
            arr = [...(echarts.history_frame || [0])].reverse()
            options[3].series[0].data = arr
          }
          // 更新历史速度
          if (this.syncProps.includes('history_speed')) {
            arr = [...(echarts.history_speed || [0])].reverse()
            options[3].series[1].data = arr
          }
        }
        // 更新当前帧率
        curEcharts.camera_frame = echarts.camera_frame

        // 遍历echarts数据，消除加载蒙层
        for (const key in echarts) {
          if (this.propsIsOk[key] === undefined) {
            this.$set(this.propsIsOk, key, true)
          }
          if (this.propsTime[key] === undefined) {
            this.$set(this.propsTime, key, 0)
          }
          // 数据更新到视图中，取消蒙层
          if (curEcharts[key] === echarts[key]) {
            this.propsIsOk[key] = true
            clearTimeout(this.propsTime[key])
          }
        }
        // 更新echarts配置数据
        for (const index in this.els) {
          if (this.stopHistoryUpdate && index == 3) {
            continue
          }
          this.myEcharts[index].setOption(options[index])
        }
      }
    },
    // 只同步需要同步的推送数据
    syncPushData(newCarState) {
      const tmpObj = {}
      for (const key in newCarState) {
        // 筛选出只存在同步集合中的数据字段
        if (this.syncProps.includes(key)) {
          tmpObj[key] = newCarState[key]
        }
      }
      // 按需更新同步的数据到echarts
      Object.assign(this.echartsData, tmpObj)
    },
    // 更新小车状态
    updateCarState(data) {
      // 更新设备连接状态,有数据则刷新连接
      this.pollMqttState()

      // 未获取到过数据，首次接收推送数据
      if (!this.statesData && this.isActive) {
        // 提示开始同步数据

        // 延迟6s后云端配置下发到小车上
        setTimeout(() => {
          this.handleSyncData('cloud')
        }, 1000)
      }
      // 保持本次接收到的数据
      this.statesData = data
      // 获取设备token，用于发送指令给设备
      this.clientToken = this.statesData.payload.clientToken
      // 根据配置的需同步的数据，更新设备数据
      this.syncPushData(this.statesData.payload.params)
    },
    // 保持websocket连接
    keepWssConnect(data) {
      const secret = localStorage.getItem('iotc_secret') || ''
      if (data === 'ping' && secret) {
        setTimeout(() => {
          this.wss.send(`pong:${secret}`)
        }, 3000)
        return true
      }
      return false
    },
    // websocket收到消息
    wssReceiveMessage(msg) {
      // 心跳包维持
      if (this.keepWssConnect(msg.data)) return
      // 非激活状态不需要更新
      // console.log(msg)
      // 状态数据
      if (typeof msg.data === 'string') {
        try {
          msg = JSON.parse(msg.data) || {} 
        } catch (error) {
          msg = {}
        }
        if (msg.type === 'states') {
          this.updateCarState(msg.data)
          if (sessionStorage.getItem('reset_wifi2')) {
            sessionStorage.removeItem('reset_wifi2')
            const params = { reset_wifi2: 1 }
            this.putProps(`/putProps`, params)
          }
        } else if (msg.type === 'connect') {
          this.pollControlState()
          this.controlIsConnected = msg.data.controlIsConnected
        }
      } else if (this.isActive) {
        // 图片数据
        // 用于保存图片
        this.preImgData = URL.createObjectURL(msg.data)
        //console.log(this.preImgData)
        document.getElementById('imgData').src = this.preImgData
        // 接收图像数据标志位
        this.hasReceiveImgData = true
      }
    },
    // 建立webSocket连接获取服务器数据
    openWebSocket() {
      // this.wss = new WebSocket(`ws://localhost:8889`)
      this.wss = new WebSocket(`wss://wlw.5102it.cn/websocket`)
      // ws连接打开回调
      this.wss.onopen = () => {
        console.log('wss：连接建立')
      }
      // ws连接收到消息回调
      this.wss.onmessage = this.wssReceiveMessage
      // ws连接出错回调
      this.wss.onerror = (e) => {
        this.wss.close()
        console.log(`wss：连接出错`)
      }
      // ws关闭连接回调
      this.wss.onclose = (code, reason) => {
        console.log(`wss：连接关闭`)
        setTimeout(() => {
          console.log(`ws：重试中...`)
          this.openWebSocket()
        }, 1000)
      }
    },
    // 映射dom元素到图表
    mapElementToEcharts() {
      this.els.push(document.getElementById('speed-whool'))
      this.els.push(document.getElementById('speed-every'))
      this.els.push(document.getElementById('duoji-angle'))
      this.els.push(document.getElementById('history-data'))
    },
    // 初始化图表
    drawAllEcharts() {
      for (const index in this.els) {
        const myEchar = this.$echarts.init(this.els[index], 'dark')
        myEchar.setOption(this.options[index])
        this.myEcharts.push(myEchar)
      }

      window.onresize = (e) => {
        this.initDrag()
        this.myEcharts.forEach((myEchar) => myEchar.resize())
      }
    },
  },
}
</script>
<style>
.option-list {
  background-color: #034079 !important;
}
.el-input.el-input--suffix,
.el-input__inner {
  display: block;
  text-align: center;
  padding: 0px !important;
  background-color: #00000000 !important;
}
.el-input__suffix {
  display: none;
}
</style>
<style scoped>
@import '~@/assets/css/home.css';
.camera-state-content /deep/ .el-input.el-input--suffix,
.camera-state-content /deep/ .el-input__inner,
.camera-state-content /deep/ .el-select-dropdown.el-popper.option-list {
  height: 22px !important;
}
</style>
