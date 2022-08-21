import Vue from "vue";

import {
Menu,MenuItem,Card,Input,Button,Switch,Slider,Select,Option,OptionGroup,Upload,Message,Notification
} from 'element-ui';
Vue.use(Menu);
Vue.use(MenuItem);
Vue.use(Card);
Vue.use(Input);
Vue.use(Button);
Vue.use(Switch);
Vue.use(Slider);
Vue.use(Select);
Vue.use(Option);
Vue.use(OptionGroup);
Vue.use(Upload);

Vue.prototype.$message = Message;
Vue.prototype.$notify = Notification;