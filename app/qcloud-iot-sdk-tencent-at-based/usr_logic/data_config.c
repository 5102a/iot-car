/*-----------------data config start  -------------------*/ 

#define TOTAL_PROPERTY_COUNT 27

static sDataPoint    sg_DataTemplate[TOTAL_PROPERTY_COUNT];

typedef struct _ProductDataDefine {
    TYPE_DEF_TEMPLATE_INT m_camera_contrast;
    TYPE_DEF_TEMPLATE_INT m_camera_saturation;
    TYPE_DEF_TEMPLATE_INT m_camera_autoexposure_level;
    TYPE_DEF_TEMPLATE_INT m_camera_effect;
    TYPE_DEF_TEMPLATE_INT m_camera_light_mode;
    TYPE_DEF_TEMPLATE_BOOL m_camera_mode;
    TYPE_DEF_TEMPLATE_BOOL m_camera_workmode;
    TYPE_DEF_TEMPLATE_INT m_camera_brightness;
    TYPE_DEF_TEMPLATE_BOOL m_camera_color_bar;
    TYPE_DEF_TEMPLATE_BOOL m_camera_is_abort;
    TYPE_DEF_TEMPLATE_INT m_camera_frame;
    TYPE_DEF_TEMPLATE_INT m_cur_duoji_2_angle;
    TYPE_DEF_TEMPLATE_INT m_cur_speed_angle;
    TYPE_DEF_TEMPLATE_INT m_cur_speed_whole;
    TYPE_DEF_TEMPLATE_INT m_set_speed_whole;
    TYPE_DEF_TEMPLATE_INT m_set_speed_angle;
    TYPE_DEF_TEMPLATE_INT m_cur_speed_rq;
    TYPE_DEF_TEMPLATE_INT m_cur_speed_lq;
    TYPE_DEF_TEMPLATE_INT m_cur_speed_rh;
    TYPE_DEF_TEMPLATE_INT m_cur_speed_lh;
    TYPE_DEF_TEMPLATE_INT m_set_speed_dir;
    TYPE_DEF_TEMPLATE_INT m_cur_duoji_1_angle;
    TYPE_DEF_TEMPLATE_BOOL m_car_control_transmission_state;
    TYPE_DEF_TEMPLATE_INT m_camera_size;
    TYPE_DEF_TEMPLATE_INT m_set_dir_speed_whole;
    TYPE_DEF_TEMPLATE_STRING m_car_secret[2048+1];
    TYPE_DEF_TEMPLATE_BOOL m_reset_wifi2;
} ProductDataDefine;

static   ProductDataDefine     sg_ProductData;

static void _init_data_template(void)
{
    sg_ProductData.m_camera_contrast = 0;
    sg_DataTemplate[0].data_property.data = &sg_ProductData.m_camera_contrast;
    sg_DataTemplate[0].data_property.key  = "camera_contrast";
    sg_DataTemplate[0].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_camera_saturation = 0;
    sg_DataTemplate[1].data_property.data = &sg_ProductData.m_camera_saturation;
    sg_DataTemplate[1].data_property.key  = "camera_saturation";
    sg_DataTemplate[1].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_camera_autoexposure_level = 0;
    sg_DataTemplate[2].data_property.data = &sg_ProductData.m_camera_autoexposure_level;
    sg_DataTemplate[2].data_property.key  = "camera_autoexposure_level";
    sg_DataTemplate[2].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_camera_effect = 0;
    sg_DataTemplate[3].data_property.data = &sg_ProductData.m_camera_effect;
    sg_DataTemplate[3].data_property.key  = "camera_effect";
    sg_DataTemplate[3].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_camera_light_mode = 0;
    sg_DataTemplate[4].data_property.data = &sg_ProductData.m_camera_light_mode;
    sg_DataTemplate[4].data_property.key  = "camera_light_mode";
    sg_DataTemplate[4].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_camera_mode = 0;
    sg_DataTemplate[5].data_property.data = &sg_ProductData.m_camera_mode;
    sg_DataTemplate[5].data_property.key  = "camera_mode";
    sg_DataTemplate[5].data_property.type = TYPE_TEMPLATE_BOOL;

    sg_ProductData.m_camera_workmode = 0;
    sg_DataTemplate[6].data_property.data = &sg_ProductData.m_camera_workmode;
    sg_DataTemplate[6].data_property.key  = "camera_workmode";
    sg_DataTemplate[6].data_property.type = TYPE_TEMPLATE_BOOL;

    sg_ProductData.m_camera_brightness = 0;
    sg_DataTemplate[7].data_property.data = &sg_ProductData.m_camera_brightness;
    sg_DataTemplate[7].data_property.key  = "camera_brightness";
    sg_DataTemplate[7].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_camera_color_bar = 0;
    sg_DataTemplate[8].data_property.data = &sg_ProductData.m_camera_color_bar;
    sg_DataTemplate[8].data_property.key  = "camera_color_bar";
    sg_DataTemplate[8].data_property.type = TYPE_TEMPLATE_BOOL;

    sg_ProductData.m_camera_is_abort = 0;
    sg_DataTemplate[9].data_property.data = &sg_ProductData.m_camera_is_abort;
    sg_DataTemplate[9].data_property.key  = "camera_is_abort";
    sg_DataTemplate[9].data_property.type = TYPE_TEMPLATE_BOOL;

    sg_ProductData.m_camera_frame = 0;
    sg_DataTemplate[10].data_property.data = &sg_ProductData.m_camera_frame;
    sg_DataTemplate[10].data_property.key  = "camera_frame";
    sg_DataTemplate[10].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_cur_duoji_2_angle = -90;
    sg_DataTemplate[11].data_property.data = &sg_ProductData.m_cur_duoji_2_angle;
    sg_DataTemplate[11].data_property.key  = "cur_duoji_2_angle";
    sg_DataTemplate[11].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_cur_speed_angle = 0;
    sg_DataTemplate[12].data_property.data = &sg_ProductData.m_cur_speed_angle;
    sg_DataTemplate[12].data_property.key  = "cur_speed_angle";
    sg_DataTemplate[12].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_cur_speed_whole = 0;
    sg_DataTemplate[13].data_property.data = &sg_ProductData.m_cur_speed_whole;
    sg_DataTemplate[13].data_property.key  = "cur_speed_whole";
    sg_DataTemplate[13].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_set_speed_whole = 0;
    sg_DataTemplate[14].data_property.data = &sg_ProductData.m_set_speed_whole;
    sg_DataTemplate[14].data_property.key  = "set_speed_whole";
    sg_DataTemplate[14].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_set_speed_angle = 0;
    sg_DataTemplate[15].data_property.data = &sg_ProductData.m_set_speed_angle;
    sg_DataTemplate[15].data_property.key  = "set_speed_angle";
    sg_DataTemplate[15].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_cur_speed_rq = 0;
    sg_DataTemplate[16].data_property.data = &sg_ProductData.m_cur_speed_rq;
    sg_DataTemplate[16].data_property.key  = "cur_speed_rq";
    sg_DataTemplate[16].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_cur_speed_lq = 0;
    sg_DataTemplate[17].data_property.data = &sg_ProductData.m_cur_speed_lq;
    sg_DataTemplate[17].data_property.key  = "cur_speed_lq";
    sg_DataTemplate[17].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_cur_speed_rh = 0;
    sg_DataTemplate[18].data_property.data = &sg_ProductData.m_cur_speed_rh;
    sg_DataTemplate[18].data_property.key  = "cur_speed_rh";
    sg_DataTemplate[18].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_cur_speed_lh = 0;
    sg_DataTemplate[19].data_property.data = &sg_ProductData.m_cur_speed_lh;
    sg_DataTemplate[19].data_property.key  = "cur_speed_lh";
    sg_DataTemplate[19].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_set_speed_dir = -1;
    sg_DataTemplate[20].data_property.data = &sg_ProductData.m_set_speed_dir;
    sg_DataTemplate[20].data_property.key  = "set_speed_dir";
    sg_DataTemplate[20].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_cur_duoji_1_angle = -135;
    sg_DataTemplate[21].data_property.data = &sg_ProductData.m_cur_duoji_1_angle;
    sg_DataTemplate[21].data_property.key  = "cur_duoji_1_angle";
    sg_DataTemplate[21].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_car_control_transmission_state = 0;
    sg_DataTemplate[22].data_property.data = &sg_ProductData.m_car_control_transmission_state;
    sg_DataTemplate[22].data_property.key  = "car_control_transmission_state";
    sg_DataTemplate[22].data_property.type = TYPE_TEMPLATE_BOOL;

    sg_ProductData.m_camera_size = 0;
    sg_DataTemplate[23].data_property.data = &sg_ProductData.m_camera_size;
    sg_DataTemplate[23].data_property.key  = "camera_size";
    sg_DataTemplate[23].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_set_dir_speed_whole = 0;
    sg_DataTemplate[24].data_property.data = &sg_ProductData.m_set_dir_speed_whole;
    sg_DataTemplate[24].data_property.key  = "set_dir_speed_whole";
    sg_DataTemplate[24].data_property.type = TYPE_TEMPLATE_INT;

    sg_ProductData.m_car_secret[0] = '\0';
    sg_DataTemplate[25].data_property.data = sg_ProductData.m_car_secret;
    sg_DataTemplate[25].data_property.key  = "car_secret";
    sg_DataTemplate[25].data_property.type = TYPE_TEMPLATE_STRING;

    sg_ProductData.m_reset_wifi2 = 0;
    sg_DataTemplate[26].data_property.data = &sg_ProductData.m_reset_wifi2;
    sg_DataTemplate[26].data_property.key  = "reset_wifi2";
    sg_DataTemplate[26].data_property.type = TYPE_TEMPLATE_BOOL;

};
