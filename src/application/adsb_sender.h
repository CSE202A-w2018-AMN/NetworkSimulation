#ifndef ADSB_SENDER_H
#define ADSB_SENDER_H

#include <ns3/application.h>
#include "device/mesh_net_device.h"
#include <functional>

/**
 * An application that periodically sends ADS-B messages
 */
class AdsBSender : public ns3::Application {
public:

    typedef std::function<void (ns3::Packet)> send_operation;

    AdsBSender() = default;
    virtual ~AdsBSender() = default;

    ns3::TimeValue GetInterval() const;
    void SetInterval(const ns3::TimeValue& interval);

    void SetSendOperation(send_operation operation);

    static ns3::TypeId GetTypeId();

private:
    /** Interval between position updates */
    ns3::TimeValue _interval;
    /** Event handle for sending */
    ns3::EventId _send_event;

    /**
     * The operation used for sending
     */
    send_operation _send_operation;

    virtual void StartApplication() override;
    virtual void StopApplication() override;

    void SendMessage();
};

#endif
