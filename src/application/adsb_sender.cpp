#include "adsb_sender.h"
#include <ns3/log.h>
#include <ns3/packet.h>
#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("AdsBSender");

ns3::TimeValue AdsBSender::GetInterval() const {
    return _interval;
}

void AdsBSender::SetInterval(const ns3::TimeValue& interval) {
    NS_LOG_FUNCTION(this << interval.Get());
    _interval = interval;
}

void AdsBSender::SetNetDevice(ns3::Ptr<MeshNetDevice> net_device) {
    NS_LOG_FUNCTION(this << net_device);
    _net_device = net_device;
}

void AdsBSender::StartApplication() {
    NS_LOG_FUNCTION(this);
    _send_event = ns3::Simulator::ScheduleNow(&AdsBSender::SendMessage, this);
}

void AdsBSender::StopApplication() {
    NS_LOG_FUNCTION(this);
    ns3::Simulator::Cancel(_send_event);
}

void AdsBSender::SendMessage() {
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(ns3::Simulator::Now() << " ADS-B sending message");
    if (_net_device) {
        const auto packet = ns3::Packet(reinterpret_cast<const std::uint8_t*>("ADSB"), 4);
        _net_device->Send(packet, IcaoAddress(0xffffff));
    } else {
        NS_LOG_WARN("No network device to send on");
    }
    _send_event = ns3::Simulator::Schedule(_interval.Get(), &AdsBSender::SendMessage, this);
}

ns3::TypeId AdsBSender::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("AdsBSender")
        .SetParent<ns3::Application>()
        .AddConstructor<AdsBSender>();
    return id;
}
