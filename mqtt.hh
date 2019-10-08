#ifndef __MQTT_HPP__
#define __MQTT_HPP__
#include <vector>
#include "mosquittopp.h"
#include "uuid.hh"

using namespace mosqpp;

class Mqtt : public mosquittopp
{
public:
    Mqtt(void);
    virtual ~Mqtt(void);

    bool Init(const std::string& version, const Uuid& uuid, const std::string& host,
                    uint16_t port, const std::string& user, const std::string& passwd);

    bool Start(void);
    void Stop(void);

    bool SubscribeEvent(const Uuid& uuid);
    bool SubscribeStatus(const Uuid& uuid);
    bool UnsubscribeEvent(const Uuid& uuid);
    bool UnsubscribeStatus(const Uuid& uuid);

    bool SubscribeMessage(const Uuid& uuid);

    virtual void OnConnect(void);
    virtual void OnDisconnect(void);

    Uuid *GetUuid(void);

protected:
    bool SendMessage(const Uuid& uuid, const std::string& data);
    bool SendEvent(const std::string& data);
    bool SendStatus(const std::string& data);

    virtual bool SendStatusDown(const std::string& data);
    virtual void OnMessage(const std::string& data);
    virtual void OnEvent(const Uuid& uuid, const std::string& data);
    virtual void OnStatus(const Uuid& uuid, const std::string& data);

    /* 继承于mosquittopp */
    void on_connect(int rc);
    void on_disconnect(int rc);
    void on_publish(int mid);
    void on_message(const struct mosquitto_message * message);
    void on_subscribe(int mid, int qos_count, const int * granted_qos);
    void on_unsubscribe(int mid);
    void on_log(int level, const char * str);
    void on_error(void);

    void SetWill(const std::string& will);
protected:
    Uuid *_uuid;

private:
    bool _connected;

    std::string _version;
    std::string _host;
    uint16_t _port;
    std::string _will;
    std::string _user;
    std::string _passwd;

    vector<std::string> _event_vec;
    vector<std::string> _status_vec;
};
#endif

