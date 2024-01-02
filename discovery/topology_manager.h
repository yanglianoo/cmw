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

private:
    std::atomic<bool> init_;
    NodeManagerPtr node_manager_;
    transport::ParticipantPtr participant_;
    ChangeSignal change_signal_;


};


}
}
}

#endif