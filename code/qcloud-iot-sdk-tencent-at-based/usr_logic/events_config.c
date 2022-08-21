#ifdef EVENT_POST_ENABLED

#define EVENT_COUNTS     (1)

static TYPE_DEF_TEMPLATE_BOOL sg_test;
static DeviceProperty g_propertyEvent_update[] = {

   {.key = "test", .data = &sg_test, .type = TYPE_TEMPLATE_BOOL},
};


static sEvent g_events[]={

    {
     .event_name = "update",
     .type = "info",
     .timestamp = 0,
     .eventDataNum = sizeof(g_propertyEvent_update)/sizeof(g_propertyEvent_update[0]),
     .pEventData = g_propertyEvent_update,
    },
};

#endif
