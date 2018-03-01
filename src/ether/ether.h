#ifndef ETHER_H
#define ETHER_H

#include <vector>
#include <ns3/node-list.h>
#include "device/mesh_net_device.h"

/**
 * The medium through which wireless messages are transmitted
 */
class Ether {
private:
    /**
     * The network devices that communicate over this medium
     */
    std::vector<ns3::Ptr<MeshNetDevice>> _devices;

    /**
     * Maximum transmission range, meters
     */
    double _range;
public:
    /**
     * Creates an Ether with unlimited range
     */
    Ether();

    /**
     * Creates an Ether from a start iterator and a past-end iterator that yield
     * Ptr<MeshNetDevice> objects
     */
    template <typename Iter>
    Ether(Iter start, Iter end) :
        _devices(start, end)
    {
        for (auto& device : _devices) {
            device->SetSendCallback(std::bind(&Ether::OnSend, this, std::placeholders::_1, std::placeholders::_2));
        }
    }

    void SetRange(double range);
    double GetRange() const;

    void AddDevice(ns3::Ptr<MeshNetDevice> device);

private:
    /**
     * Called from network devices when messages are sent
     */
    void OnSend(const ns3::Vector& position, ns3::Packet packet);
};

#endif
