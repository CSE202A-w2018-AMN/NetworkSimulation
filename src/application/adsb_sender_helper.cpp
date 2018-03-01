#include "adsb_sender_helper.h"
#include "application/adsb_sender.h"

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
        application->SetNetDevice(node->GetObject<MeshNetDevice>());
        node->AddApplication(application);
        apps.Add(application);
    }
    return apps;
}
