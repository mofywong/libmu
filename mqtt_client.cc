#include <he/log.h>
#include "mqtt_client.hh"
#include "he.pb.h"
using namespace He;

/* 请求等待回复的超时时间，ms */
#define RESPONSE_TIME_OUT   30000;

MqttClient::MqttClient(void)
:Mqtt()
{
    _seq = 0;
    _response_timeout_timer.SetTimeInterval(1000);

    std::string strWill("");
    He::Status objStatus;
    objStatus.set_type(He::STS_TYPE_DOWN);
    objStatus.set_data("");
    objStatus.SerializeToString(&strWill);
    SetWill(strWill);

    _response_timeout_timer.Start();
}

MqttClient::~MqttClient(void) {

}

bool MqttClient::SendMessageLinkEnroll(const Uuid& uuid,
                                const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MSG_TYPE_LINK_ENROLL);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(Seq());
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);

    bRet = SendMessage(uuid, strMessage);

    return bRet;
}

bool MqttClient::SendMessageDeviceEnroll(const Uuid& uuid,
                                  const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MSG_TYPE_DEVICE_ENROLL);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(Seq());
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);
    bRet = SendMessage(uuid, strMessage);

    return bRet;

}

bool MqttClient::SendMessageGatewayEnroll(const Uuid& uuid, const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MSG_TYPE_GATEWAY_ENROLL);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(Seq());
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);
    bRet = SendMessage(uuid, strMessage);

    return bRet;

}

void MqttClient::AssociatedResponse(uint32_t seq, uint32_t type)
{
    std::function<void(uint32_t, uint32_t, const std::string&)> response_fun;

    switch (type)
    {
        case He::METH_TYPE_GET_GATEWAY:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetGateway,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_GET_THINGS:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetThings,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_TIME:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetTime,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ON_OFF:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetOnOff,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ON_OFF_DELAY:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetOnOffDelay,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ON_OFF_DAILY:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetOnOffDaily,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ON_OFF_WEEKLY:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetOnOffWeekly,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_LIST_TIME_TASK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseListTimeTask,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_CANCEL_TIME_TASK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseCancelTimeTask,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_CLEAR_TIME_TASK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseClearTimeTask,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_CLEAR_ENERGY_STATISTICS:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseClearEnergyStatistics,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_LEVEL:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetLevel,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_BRIGHTNESS:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetBrightness,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_ADD_IPC:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseAddIpc,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_REMOVE_IPC:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseRemoveIpc,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ACCOUNT:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetAccount,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_MOVE_PTZ:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseMovePtz,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SAVE_PTZ:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSavePtz,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_RESTORE_PTZ:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseRestorePtz,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_LIST_PTZ:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseListPtz,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_REMOVE_PTZ:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseRemovePtz,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_GET_CPU_USAGE:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetCpuUsage,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_GET_RAM_USAGE:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetRamUsage,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_GET_TOTAL_RX_BYTES:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetTotalRxBytes,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_GET_TOTAL_TX_BYTES:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetTotalTxBytes,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_REBOOT:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseReboot,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SCREENSHOT:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseScreenShot,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_LOAD_URL:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseLoadUrl,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_PLAY_FILE:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponsePlayFile,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_UPDATE:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseUpdate,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ATTRIBUTE_THRESHOLD:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetAttributeThreshold,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ATTRIBUTE_INTERVAL:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetAttributeInterval,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_GET_CONFIG:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetConfig,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_CONFIG:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetConfig,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_ADD_DIO_LOCK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseAddDioLock,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_REMOVE_DIO_LOCK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseRemoveDioLock,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_PAUSE:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponsePause,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_RESUME:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseResume,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_GET_GATEWAY_UUID:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetGatewayUuid,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_VOLUME:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetVolume,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_FOCUS:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetFocus,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_APERTURE:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetAperture,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_PUSH_VIDEO_PREVIEW:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponsePushVideoPreview,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_PULL_VIDEO_PREVIEW:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponsePullVideoPreview,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_STOP_VIDEO_PREVIEW:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseStopVideoPreview,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_GET_VIDEO_STREAM:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseGetVideoStream,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_STILL_CAPTURE:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseStillCapture,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_ADD_STREET_LAMP:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseAddStreetLamp,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_REMOVE_STREET_LAMP:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseRemoveStreetLamp,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_ADD_MODBUS_RTU:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseAddModbusRtu,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_REMOVE_MODBUS_RTU:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseRemoveModbusRtu,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_UART:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetUart,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_LOAD_PROGRAM:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseLoadProgram,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_STOP_PTZ:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseStopPtz,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_CLEAR_PTZ:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseClearPtz,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }

        case He::METH_TYPE_ADD_CRUISE_TRACK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseAddCruiseTrack,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_REMOVE_CRUISE_TRACK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseRemoveCruiseTrack,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_ACTIVE_CRUISE_TRACK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseActiveCruiseTrack,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_INACTIVE_CRUISE_TRACK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseInactiveCruiseTrack,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_LIST_CRUISE_TRACK:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseListCruiseTrack,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ZOOM:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetZoom,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_IDLE:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseIdle,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_BRIGHTNESS_WITH_TIME:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetBrightnessWithTime,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ON_OFF_WITH_TIME:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetOnOffWithTime,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_LOAD_PROGRAM_WITH_TIME:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseLoadProgramWithTime,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_LOAD_TEXT:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseLoadText,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ONOFF_WITH_RESUME:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetOnOffWithResume,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_BRIGHTNESS_WITH_RESUME:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseSetBrightnessWithResume,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_ENABLE_TIME_POLICY:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseEnableTimePolicy,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }

        case He::METH_TYPE_SET_ANGLE_AXIS_ORIGIN:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_OVER_VOLTAGE_THRESHOLD:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_LOW_VOLTAGE_THRESHOLD:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_OVER_CURRENT_THRESHOLD:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_LEAKAGE_CURRENT_THRESHOLD:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ATTRIBUTE_CHANGE_THRESHOLD:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SET_ATTRIBUTE_CHANGE_INTERVAL:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_PRELOAD_PROGRAM:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }
        case He::METH_TYPE_SWITCH_PROGRAM:
        {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                                     this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3);
            break;
        }

         case He::METH_TYPE_LOAD_PROGRAM_WITH_DATE: {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                 this, std::placeholders::_1, std::placeholders::_2,
                     std::placeholders::_3);
            break;
        }    
        case He::METH_TYPE_DELETE_LOAD_PROGRAM_WITH_DATE: {
            response_fun = std::bind(&MqttClient::OnMessageResponseNoOut,
                 this, std::placeholders::_1, std::placeholders::_2,
                     std::placeholders::_3);
            break;
        }       

        default:
        {
            return;
        }
    }
    seq_response_fun_map.insert(make_pair(seq, response_fun));

}


uint32_t MqttClient::SendMessageInvokeMethod(const Uuid& uuid,
                                  const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());
    He::Message objMesg;

    objMesg.set_type(He::MSG_TYPE_INVOKE_METHOD);
    objMesg.set_src(strSelfUu);

    uint32_t iSeq = Seq();

    /* 关联on response */
    He::MessageInvokeMethod message_invoke;
    message_invoke.ParseFromArray(data.data(), data.size());
    He::MethodType type = message_invoke.type();
    AssociatedResponse(iSeq, type);

    He::TimerSource timer_source;
    timer_source.time_out_ms = RESPONSE_TIME_OUT;

    timer_source.fun = std::bind(&MqttClient::OnMessageResponseTimeOut, this, std::placeholders::_1);

    char seq[24] = {0};
    sprintf(seq, "%u", iSeq);
    timer_source.strID = seq;
    _response_timeout_timer.AddMonitor(timer_source);

    objMesg.set_seq(iSeq);
    objMesg.set_data(data);
    objMesg.SerializeToString(&strMessage);

    bRet = SendMessage(uuid, strMessage);
    if (!bRet)
        return 0;
    return iSeq;
}

bool MqttClient::SendMessageResponseMethod(const Uuid& uuid,
                      uint32_t seq, const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MSG_TYPE_RESPONSE_METHOD);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(seq);
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);

    bRet = SendMessage(uuid, strMessage);

    return bRet;
}

bool MqttClient::SendMessageAttributeNotify(const Uuid& uuid,
                                     const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MessageType::MSG_TYPE_ATTRIBUTE_NOTIFY);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(Seq());
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);

    bRet = SendMessage(uuid, strMessage);

    return bRet;
}

bool MqttClient::SendMessageSignalNotify(const Uuid& uuid,
                                  const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MessageType::MSG_TYPE_SIGNAL_NOTIFY);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(Seq());
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);

    bRet = SendMessage(uuid, strMessage);

    return bRet;
}

bool MqttClient::SendMessageOnlineNotify(const Uuid& uuid,
                                  const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MessageType::MSG_TYPE_ONLINE_NOTIFY);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(Seq());
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);

    bRet = SendMessage(uuid, strMessage);

    return bRet;

}

bool MqttClient::SendMessageOfflineNotify(const Uuid& uuid,
                                   const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MessageType::MSG_TYPE_OFFLINE_NOTIFY);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(Seq());
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);

    bRet = SendMessage(uuid, strMessage);

    return bRet;

}

bool MqttClient::SendMessageReadyNotify(const Uuid& uuid, const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Message objMesg;
    std::string strSelfUu((char*)_uuid->Data(), _uuid->Size());

    objMesg.set_type(He::MessageType::MSG_TYPE_READY_NOTIFY);
    objMesg.set_src(strSelfUu);
    objMesg.set_seq(Seq());
    objMesg.set_data(data);

    objMesg.SerializeToString(&strMessage);

    bRet = SendMessage(uuid, strMessage);

    return bRet;
}
bool MqttClient::SendEventAttributeChanged(const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Event objEvent;

    objEvent.set_type(He::EventType::EVT_TYPE_ATTRIBUTE_CHANGED);
    objEvent.set_seq(Seq());
    objEvent.set_data(data);

    objEvent.SerializeToString(&strMessage);

    bRet = SendEvent(strMessage);

    return bRet;
}

bool MqttClient::SendEventSignalEmitted(const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Event objEvent;

    objEvent.set_type(He::EventType::EVT_TYPE_SIGNAL_EMITTED);
    objEvent.set_seq(Seq());
    objEvent.set_data(data);

    objEvent.SerializeToString(&strMessage);

    bRet = SendEvent(strMessage);

    return bRet;
}

bool MqttClient::SendEventThingOnline(const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Event objEvent;

    objEvent.set_type(He::EventType::EVT_TYPE_THING_ONLINE);
    objEvent.set_seq(Seq());
    objEvent.set_data(data);

    objEvent.SerializeToString(&strMessage);

    bRet = SendEvent(strMessage);

    return bRet;
}

bool MqttClient::SendEventThingOffline(const std::string& data)
{
    bool bRet = false;
    std::string strMessage("");
    He::Event objEvent;

    objEvent.set_type(He::EventType::EVT_TYPE_THING_OFFLINE);
    objEvent.set_seq(Seq());
    objEvent.set_data(data);

    objEvent.SerializeToString(&strMessage);

    bRet = SendEvent(strMessage);

    return bRet;
}

bool MqttClient::SendStatusUp(const string& data)
{
    he_utils_log_info("MqttClient::SendStatusUp\n");
    bool bRet = false;
    std::string strStatus("");
    He::Status objStatus;

    objStatus.set_type(He::STS_TYPE_UP);
    objStatus.set_data(data);

    objStatus.SerializeToString(&strStatus);

    bRet = SendStatus(strStatus);

    return bRet;
}

bool MqttClient::SendStatusDown(const string& data)
{
    he_utils_log_info("MqttClient::SendStatusDown\n");
    bool bRet = false;
    std::string strStatus("");
    He::Status objStatus;

    objStatus.set_type(He::STS_TYPE_DOWN);
    objStatus.set_data(data);

    objStatus.SerializeToString(&strStatus);

    bRet = SendStatus(strStatus);

    return bRet;

}

bool MqttClient::SendStatusReady(const string& data)
{
    bool bRet = false;
    std::string strStatus("");
    He::Status objStatus;

    objStatus.set_type(He::STS_TYPE_READY);
    objStatus.set_data(data);

    objStatus.SerializeToString(&strStatus);

    bRet = SendStatus(strStatus);

    return bRet;

}

void MqttClient::OnConnect(void)
{

}

void MqttClient::OnDisconnect(void)
{

}


bool MqttClient::SubscribeStatusGateway(void)
{
    bool bRet = false;
    //uuid_t uuid;
    //memcpy(uuid, GW_UUID_INTERNAL, sizeof(GW_UUID_INTERNAL));
    Uuid objUuid;
    objUuid.From(GW_UUID_INTERNAL);
    bRet = SubscribeStatus(objUuid);

    return bRet;
}

bool MqttClient::UnsubscribeStatusGateway(void)
{
    bool bRet = false;
    //uuid_t uuid;
    //memcpy(uuid, GW_UUID_INTERNAL, sizeof(GW_UUID_INTERNAL));
    Uuid objUuid;
    objUuid.From(GW_UUID_INTERNAL);
    bRet = UnsubscribeStatus(objUuid);

    return bRet;
}


bool MqttClient::SubscribeStatusDevice(const Uuid& uuid)
{
    bool bRet = SubscribeStatus(uuid);
    return bRet;
}

bool MqttClient::UnsubscribeStatusDevice(const Uuid& uuid)
{
    bool bRet = UnsubscribeStatus(uuid);
    return bRet;
}


bool MqttClient::SubscribeStatusLink(const Uuid& uuid)
{
    bool bRet = SubscribeStatus(uuid);
    return bRet;
}

bool MqttClient::UnsubscribeStatusLink(const Uuid& uuid)
{
    bool bRet = UnsubscribeStatus(uuid);
    return bRet;
}


bool MqttClient::SubscribeEventDevice(const Uuid& uuid)
{
    bool bRet = SubscribeEvent(uuid);
    return bRet;
}

bool MqttClient::UnsubscribeEventDevice(const Uuid& uuid)
{
    bool bRet = UnsubscribeEvent(uuid);
    return bRet;
}


void MqttClient::OnStatusGateway(bool up)
{
    // do by subcalss
}

void MqttClient::OnStatusDevice(const Uuid& uuid, bool up)
{
    // do by subcalss
}

void MqttClient::OnStatusLink(const Uuid& uuid, uint32_t status)
{
    // do by subcalss
}

void MqttClient::OnMessageLinkEnroll(const std::string& data)
{
    // do by subcalss
}

void MqttClient::OnMessageDeviceEnroll(const std::string& data)
{
    // do by subcalss
}

void MqttClient::OnMessageGatewayEnroll(const std::string& data)
{
    // do by subcalss
}


void MqttClient::OnMessageResponse(uint32_t seq, const std::string& data)
{
    char string_seq[24] = {0};
    sprintf(string_seq, "%u", seq);
    _response_timeout_timer.RemoveMonitor(string_seq);

    He::MessageResponseMethod message_response;
    message_response.ParseFromArray(data.data(), data.size());
    uint32_t status = message_response.status();
    std::string out =  message_response.out();

    std::map<uint32_t, std::function<void(uint32_t, uint32_t, const std::string&)>>::iterator it = seq_response_fun_map.find(seq);
    if (seq_response_fun_map.end() != it)
    {
        it->second(seq, status, out);
    }
}

bool MqttClient::OnMessageInvokeMethod(const Uuid& src_uuid, uint32_t seq,
                                  const std::string& data, Uuid& dest_uuid)
{
    return false;
}

void MqttClient::OnMessageAttributeNotify(const std::string& data)
{
    // do by subcalss
}

void MqttClient::OnMessageSignalNotify(const std::string& data)
{
    // do by subcalss

}

void MqttClient::OnMessageOnlineNotify(const std::string& data)

{
    // do by subcalss

}

void MqttClient::OnMessageOfflineNotify(const std::string& data)
{
    // do by subcalss

}

void MqttClient::OnMessageReadyNotify(const Uuid& src_uuid)
{
    // do by subcalss
}

void MqttClient::OnEventAttributeChanged(const Uuid& uuid,
                                  const std::string& data)
{
    // do by subcalss

}

void MqttClient::OnEventSignalEmitted(const Uuid& uuid,
                               const std::string& data)
{
    // do by subcalss

}

void MqttClient::OnEventThingOnline(const Uuid& uuid,
                             const std::string& data)
{
    // do by subcalss

}

void MqttClient::OnEventThingOffline(const Uuid& uuid,
                              const std::string& data)
{
    // do by subcalss
}

uint32_t MqttClient::Seq(void)
{
    if (0 == _seq++)
    {
        _seq = 1;
    }
    return _seq;
}

void MqttClient::OnMessage(const std::string& data)
{
    He::Message objMesg;
    objMesg.ParseFromArray(data.data(), data.size());
    switch (objMesg.type())
    {
        case He::MSG_TYPE_LINK_ENROLL:
            {
                he_utils_log_info("OnMessage link enroll\n");
                OnMessageLinkEnroll(objMesg.data());
                break;
            }
        case He::MSG_TYPE_DEVICE_ENROLL:
            {
                he_utils_log_info("OnMessage device enroll\n");
                OnMessageDeviceEnroll(objMesg.data());
                break;
            }
        case He::MSG_TYPE_GATEWAY_ENROLL:
            {
                he_utils_log_info("OnMessage gateway enroll\n");
                OnMessageGatewayEnroll(objMesg.data());
                break;
            }
        case He::MSG_TYPE_INVOKE_METHOD:
            {
                he_utils_log_info("OnMessage invoke\n");
                const std::string &strSrcUu = objMesg.src();
                uu_t uu;
                memcpy(uu, strSrcUu.data(), UU_SIZE);
                Uuid objSrcUuid;
                if (!objSrcUuid.From(uu))
                {
                    return;
                }

                Uuid objDestUuid;
                /* out dest_uuid if this mesg is not belong to itself when bNeedTransfer is true */
                bool bNeedTransfer = OnMessageInvokeMethod(objSrcUuid, objMesg.seq(), objMesg.data(), objDestUuid);
                if (bNeedTransfer)
                {
                    SendMessage(objDestUuid, data);
                }
                he_utils_log_err("OnMessage invoke return\n");
                break;
            }
        case He::MSG_TYPE_RESPONSE_METHOD:
            {
                he_utils_log_info("OnMessage response\n");
                uint32_t iSeq = objMesg.seq();
                OnMessageResponse(iSeq, objMesg.data());
                break;
            }
        case He::MSG_TYPE_ONLINE_NOTIFY:
            {
                he_utils_log_info("OnMessage online notify\n");
                OnMessageOnlineNotify(objMesg.data());

                break;
            }
        case He::MSG_TYPE_OFFLINE_NOTIFY:
            {
                he_utils_log_info("OnMessage offline notify\n");
                OnMessageOfflineNotify(objMesg.data());

                break;
            }
        case He::MSG_TYPE_ATTRIBUTE_NOTIFY:
            {
                // he_utils_log_info("OnMessage attribute notify\n");
                OnMessageAttributeNotify(objMesg.data());

                break;
            }
        case He::MSG_TYPE_SIGNAL_NOTIFY:
            {
                he_utils_log_info("OnMessage signal notify\n");
                OnMessageSignalNotify(objMesg.data());

                break;
            }
        case He::MSG_TYPE_READY_NOTIFY:
            {
                he_utils_log_info("OnMessage ready notify\n");
                const std::string &strSrcUu = objMesg.src();
                uu_t uu;
                memcpy(uu, strSrcUu.data(), UU_SIZE);
                Uuid objSrcUuid;
                if (!objSrcUuid.From(uu))
                {
                    return;
                }

                OnMessageReadyNotify(objSrcUuid);

                break;
            }
        default:
            {
                he_utils_log_err("OnMessage type(%d) error\n", objMesg.type());
                return;
            }
    }

    return;
}

void MqttClient::OnEvent(const Uuid& uuid, const std::string& data)
{
    He::Event objEvent;
    objEvent.ParseFromString(data);

    switch ((int)objEvent.type())
    {
        case He::EVT_TYPE_THING_ONLINE:
        {
            he_utils_log_info("OnEvent thing online\n");
            OnEventThingOnline(uuid, objEvent.data());
            break;
        }
        case He::EVT_TYPE_THING_OFFLINE:
        {
            he_utils_log_info("OnEvent thing offline\n");
            OnEventThingOffline(uuid, objEvent.data());
            break;
        }
        case He::EVT_TYPE_ATTRIBUTE_CHANGED:
        {
            // he_utils_log_info("OnEvent attribute changed\n");
            OnEventAttributeChanged(uuid, objEvent.data());
            break;
        }
        case He::EVT_TYPE_SIGNAL_EMITTED:
        {
            he_utils_log_info("OnEvent signal emited\n");
            OnEventSignalEmitted(uuid, objEvent.data());
            break;
        }
        default:
        {
            he_utils_log_err("OnEvent type(%d) error\n", (int)objEvent.type());
            return;
        }

    }

    return;
}

void MqttClient::OnStatus(const Uuid& uuid, const std::string& data)
{
    uint16_t type = uuid.GetType();

    He::Status objStatus;
    objStatus.ParseFromString(data);

    bool up = false;
    if (He::STS_TYPE_UP == objStatus.type())
    {
        up = true;
    }

    switch (type)
    {
        case UUID_TYPE_GATEWAY:
        {
            he_utils_log_info("OnStatus gateway:%d\n", up);
            OnStatusGateway(up);
            break;
        }
        case UUID_TYPE_DEVICE_AGENT:
        {
            he_utils_log_info("OnStatus device agent:%d\n", up);
            OnStatusDevice(uuid, up);
            break;
        }
        case UUID_TYPE_LINK_AGENT:
        {
            he_utils_log_info("OnStatus link:%d\n", up);
            OnStatusLink(uuid, (uint32_t)objStatus.type());
            break;
        }
        default:
        {
            he_utils_log_err("OnStatus type(%d) err\n", type);
            return;
        }
    }
    return;
}

void MqttClient::OnMessageResponseTimeOut(uint32_t seq)
{
    he_utils_log_err("\033[1;31;40mMqttClient::OnMessageResponseTimeOut\n\033[0m");
    // do by subclass
}
