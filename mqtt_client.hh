#ifndef __MQTT_CLIENT_HH__
#define __MQTT_CLIENT_HH__
#include <map>
#include "mqtt.hh"
#include "timer.hh"
#define GW_UUID_INTERNAL    "1000-0000000000000000-00000000-00-ef"
namespace He
{
class MqttClient : public Mqtt
{
public:
    MqttClient(void);
    virtual ~MqttClient(void);

protected:
    /* 发送连接代理登记消息 */
    bool SendMessageLinkEnroll(const Uuid& uuid, const std::string& data);
    /* 发送设备代理登记消息 */
    bool SendMessageDeviceEnroll(const Uuid& uuid, const std::string& data);
    /* 发送网关登记消息 */
    bool SendMessageGatewayEnroll(const Uuid& uuid, const std::string& data);
    /* 发送请求调用消息 */
    uint32_t SendMessageInvokeMethod(const Uuid& uuid, const std::string& data);
    /* 发送mesg调用方法的  回复处理 */
    bool SendMessageResponseMethod(const Uuid& uuid, uint32_t seq,
                                   const std::string& data);
    /* GW->LINK，网关代理将设备代理发过来的通知通过msg发给连接代理 */
    bool SendMessageAttributeNotify(const Uuid& uuid, const std::string& data);
    bool SendMessageSignalNotify(const Uuid& uuid, const std::string& data);
    bool SendMessageOnlineNotify(const Uuid& uuid, const std::string& data);
    bool SendMessageOfflineNotify(const Uuid& uuid, const std::string& data);

    bool SendMessageReadyNotify(const Uuid& uuid, const std::string& data);

    /* 发送属性变化的事件 */
    bool SendEventAttributeChanged(const std::string& data);
    bool SendEventSignalEmitted(const std::string& data);
    /* 发送设备上线事件 */
    bool SendEventThingOnline(const std::string& data);
    /* 发送设备下线事件 */
    bool SendEventThingOffline(const std::string& data);

    bool SendStatusUp(const std::string& data);
    bool SendStatusDown(const std::string& data);
    bool SendStatusReady(const std::string& data);

    /* (反)订阅网关状态 */
    bool SubscribeStatusGateway(void);
    bool UnsubscribeStatusGateway(void);
    /* (反)订阅设备代理状态 */
    bool SubscribeStatusDevice(const Uuid& uuid);
    bool UnsubscribeStatusDevice(const Uuid& uuid);
    /* (反)订阅连状态 */
    bool SubscribeStatusLink(const Uuid& uuid);
    bool UnsubscribeStatusLink(const Uuid& uuid);
    /* (反)订阅设备代理事件 */
    bool SubscribeEventDevice(const Uuid& uuid);
    bool UnsubscribeEventDevice(const Uuid& uuid);

    /* 收到网关状态变化的处理 */
    virtual void OnStatusGateway(bool up);
    /* 收到设备代理状态变化的处理 */
    virtual void OnStatusDevice(const Uuid& uuid, bool up);
    /* 收到连接状态变化的处理 */
    virtual void OnStatusLink(const Uuid& uuid, uint32_t status);

    /* 处理连接代理注册的消息 */
    virtual void OnMessageLinkEnroll(const std::string& data);
    /* 收到设备代理注册消息处理 */
    virtual void OnMessageDeviceEnroll(const std::string& data);
    virtual void OnMessageGatewayEnroll(const std::string& data);
    /* 请求回复超时处理方法 */
    virtual void OnMessageResponseTimeOut(uint32_t seq);
    /* 处理调用请求,return true需要将数据转发给dest_uuid，dest_uuid是出参，当需要转发时，dest_uuid
    返回下一步需要转发的目的UUID*/
    virtual bool OnMessageInvokeMethod(const Uuid& src_uuid, uint32_t seq,
                                       const std::string& data, Uuid& dest_uuid);

    /* link代理处理网关转发过来的msg */
    virtual void OnMessageAttributeNotify(const std::string& data);
    virtual void OnMessageSignalNotify(const std::string& data);
    virtual void OnMessageOnlineNotify(const std::string& data);
    virtual void OnMessageOfflineNotify(const std::string& data);
    virtual void OnMessageReadyNotify(const Uuid& src_uuid);

    /* 处理属性变化的事件 */
    virtual void OnEventAttributeChanged(const Uuid& uuid, const std::string& data);
    /* 处理信号发射的事件 */
    virtual void OnEventSignalEmitted(const Uuid& uuid, const std::string& data);
    /* 处理设备上线的事件 */
    virtual void OnEventThingOnline(const Uuid& uuid, const std::string& data);
    /* 处理设备下线的事件 */
    virtual void OnEventThingOffline(const Uuid& uuid, const std::string& data);

    virtual void OnConnect(void);
    virtual void OnDisconnect(void);

    /* OnResponse */
    virtual void OnMessageResponseGetGateway(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseGetThings(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetTime(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetOnOff(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetOnOffDelay(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetOnOffDaily(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetOnOffWeekly(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseListTimeTask(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseCancelTimeTask(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseClearTimeTask(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseClearEnergyStatistics(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetLevel(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetBrightness(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseAddIpc(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseRemoveIpc(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetAccount(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseMovePtz(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSavePtz(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseRestorePtz(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseListPtz(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseRemovePtz(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseGetCpuUsage(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseGetRamUsage(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseGetTotalRxBytes(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseGetTotalTxBytes(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseReboot(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseScreenShot(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseLoadUrl(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponsePlayFile(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseUpdate(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetAttributeThreshold(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetAttributeInterval(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseGetConfig(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetConfig(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseAddDioLock(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseRemoveDioLock(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponsePause(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseResume(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseIdle(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseGetGatewayUuid(uint32_t seq, uint32_t status, const std::string& data){}

    virtual void OnMessageResponseSetVolume(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetFocus(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetAperture(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponsePushVideoPreview(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponsePullVideoPreview(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseStopVideoPreview(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseGetVideoStream(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseStillCapture(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseAddStreetLamp(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseRemoveStreetLamp(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseAddModbusRtu(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseRemoveModbusRtu(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetUart(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseLoadProgram(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseStopPtz(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseClearPtz(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseAddCruiseTrack(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseRemoveCruiseTrack(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseActiveCruiseTrack(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseInactiveCruiseTrack(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseListCruiseTrack(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetZoom(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseLoadProgramWithTime(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetOnOffWithTime(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetBrightnessWithTime(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseLoadText(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetOnOffWithResume(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseSetBrightnessWithResume(uint32_t seq, uint32_t status, const std::string& data){}
    virtual void OnMessageResponseEnableTimePolicy(uint32_t seq, uint32_t status, const std::string& data){}

    virtual void OnMessageResponseNoOut(uint32_t seq, uint32_t status, const std::string& data){}
private:
    uint32_t Seq(void);

    void OnMessage(const std::string& data);
    void OnEvent(const Uuid& uuid, const std::string& data);
    void OnStatus(const Uuid& uuid, const std::string& data);

    void OnMessageResponse(uint32_t seq, const std::string& data);
    void AssociatedResponse(uint32_t seq, uint32_t type);

private:
    uint32_t _seq;
    He::Timer _response_timeout_timer;
    std::map<uint32_t, std::function<void(uint32_t, uint32_t, const std::string&)>> seq_response_fun_map;
};
}
#endif

