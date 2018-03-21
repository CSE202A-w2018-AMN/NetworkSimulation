#include "network_protocol.h"

ns3::TypeId NetworkProtocol::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("NetworkProtocol")
        .SetParent<Object>();
    return id;
}
