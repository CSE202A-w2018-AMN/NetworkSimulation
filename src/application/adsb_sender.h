#ifndef ADSB_SENDER_H
#define ADSB_SENDER_H

#include <ns3/application.h>
#include "device/mesh_net_device.h"

/**
 * An application that periodically sends ADS-B messages
 */
class AdsBSender : public ns3::Application {
public:

    AdsBSender() = default;
    virtual ~AdsBSender() = default;

    ns3::TimeValue GetInterval() const;
    void SetInterval(const ns3::TimeValue& interval);

    void SetNetDevice(ns3::Ptr<MeshNetDevice> net_device);

    static ns3::TypeId GetTypeId();

private:
    /** Interval between position updates */
    ns3::TimeValue _interval;
    /** Event handle for sending */
    ns3::EventId _send_event;

    /**
     * The network device used for sending
     */
    ns3::Ptr<MeshNetDevice> _net_device;

    virtual void StartApplication() override;
    virtual void StopApplication() override;

    void SendMessage();
};

#endif
