#ifndef CYBER_DISCOVERY_SPECIFIC_MANAGER_MANAGER_H_
#define CYBER_DISCOVERY_SPECIFIC_MANAGER_MANAGER_H_

#include <atomic>
#include <functional>
#include <mutex>
#include <string>

#include <cmw/base/signal.h>
#include <cmw/config/topology_change.h>
#include <cmw/discovery/communication/reader_listener.h>

#include <fastrtps/rtps/reader/RTPSReader.h>
#include <fastrtps/rtps/writer/RTPSWriter.h>
#include <fastrtps/rtps/rtps_fwd.h>
namespace hnu {
namespace cmw {
namespace discovery{ 

using namespace config;
class Manager
{
public:
    using ChangeSignal = base::Signal<const ChangeMsg&>;
    using ChangeFunc = std::function<void(const ChangeMsg&)>;
    using ChangeConnection = base::Connection<const ChangeMsg&>;

    using RtpsParticipant =  eprosima::fastrtps::rtps::RTPSParticipant;

protected:

    bool CreateWriter(RtpsParticipant* participant);
    bool CreateReader(RtpsParticipant* participant);
    
    virtual bool Check(const RoleAttributes& attr) = 0;

    void Notify(const ChangeMsg& msg);



    std::atomic<bool> is_shutdown;
    std::atomic<bool> is_discovery_started_;
    int allowed_role_;
    ChangeType change_type_;
    std::string host_name_;
    int process_id_;
    std::string channel_name_;
    std::mutex lock_;

    eprosima::fastrtps::rtps::RTPSWriter* writer_;
    eprosima::fastrtps::rtps::RTPSReader* reader_;

    ReaderListener* listener_;
    ChangeSignal signal_;
};




}
}
}

#endif