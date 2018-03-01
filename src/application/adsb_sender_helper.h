#ifndef ADSB_SENDER_HELPER_H
#define ADSB_SENDER_HELPER_H

#include <ns3/application-container.h>
#include <ns3/node-container.h>

/**
 * A helper that installs ADS-B sender applications
 */
class AdsBSenderHelper {
private:
    /** The interval between transmissions to assign to applications */
    ns3::TimeValue _interval;

public:
    AdsBSenderHelper();
    AdsBSenderHelper(const ns3::TimeValue& interval);

    void SetInterval(const ns3::TimeValue& interval);

    /**
     * Creates an application for each node, assigns nodes to applications,
     * and returns the applications
     */
    ns3::ApplicationContainer Install(ns3::NodeContainer nodes);
};

#endif
