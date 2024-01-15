#ifndef CMW_SERVICE_DISCOVERY_TOPOLOGY_MANAGER_H_
#define CMW_SERVICE_DISCOVERY_TOPOLOGY_MANAGER_H_


#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>


#include <cmw/base/signal.h>
#include <cmw/common/macros.h>
#include <cmw/discovery/specific_manager/node_manager.h>
#include <cmw/transport/rtps/participant.h>
#include <cmw/discovery/communication/participant_listener.h>

namespace hnu {
namespace cmw {
namespace discovery{ 


class NodeManager;
using NodeManagerPtr = std::shared_ptr<NodeManager>;

class TopologyManager {


public:
    using ChangeSignal = base::Signal<const ChangeMsg&>;
    using ChangeFunc = std::function<void(const ChangeMsg&)>;
    using ChangeConnection = base::Connection<const ChangeMsg&>;

    using PartNameContainer = std::map<eprosima::fastrtps::rtps::GUID_t, std::string>;
    using PartInfo = eprosima::fastrtps::rtps::ParticipantDiscoveryInfo;

private:

    bool Init();
    bool InitNodeManager();
    bool CreateParticipant();

    void OnParticipantChange(const PartInfo& info);
    bool Convert(const PartInfo& info, ChangeMsg* change_msg);

    bool ParseParticipantName(const std::string& participant_name,
                            std::string* host_name, int* process_id);
    std::atomic<bool> init_;
    NodeManagerPtr node_manager_;
    transport::ParticipantPtr participant_;
    ParticipantListener* participant_listener_;
    ChangeSignal change_signal_;

    PartNameContainer participant_names_;

    DECLARE_SINGLETON(TopologyManager)
};


}
}
}

#endif