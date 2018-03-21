#include "adsb_sender_helper.h"
#include "application/adsb_sender.h"
#include "network/network_protocol.h"
#include <cassert>

AdsBSenderHelper::AdsBSenderHelper() :
    AdsBSenderHelper(ns3::TimeValue())
{
}

AdsBSenderHelper::AdsBSenderHelper(const ns3::TimeValue& interval) :
    _interval(interval)
{
}

void AdsBSenderHelper::SetInterval(const ns3::TimeValue& interval) {
    _interval = interval;
}

ns3::ApplicationContainer AdsBSenderHelper::Install(ns3::NodeContainer nodes) {
    ns3::ApplicationContainer apps;
    for (auto iter = nodes.Begin(); iter != nodes.End(); ++iter) {
        ns3::Ptr<ns3::Node> node = *iter;
        auto application = ns3::CreateObject<AdsBSender>();
        application->SetInterval(_interval);

        // Send operation uses OLSR
        auto protocol = node->GetObject<NetworkProtocol>();
        assert(protocol);
        application->SetSendOperation([protocol](ns3::Packet packet) { protocol->Send(packet, IcaoAddress(0x800000)); });
        node->AddApplication(application);
        apps.Add(application);
    }
    return apps;
}
