#include "adsb_sender.h"
#include <ns3/log.h>
#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("AdsBSender");

ns3::TimeValue AdsBSender::GetInterval() const {
    return _interval;
}

void AdsBSender::SetInterval(const ns3::TimeValue& interval) {
    NS_LOG_FUNCTION(this << interval.Get());
    _interval = interval;
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

    _send_event = ns3::Simulator::Schedule(_interval.Get(), &AdsBSender::SendMessage, this);
}

ns3::TypeId AdsBSender::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("AdsBSender")
        .SetParent<ns3::Application>()
        .AddConstructor<AdsBSender>();
    return id;
}
