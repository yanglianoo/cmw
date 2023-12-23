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
#include <fastrtps/rtps/RTPSDomain.h>
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

    Manager();
    virtual ~Manager();
    virtual void Shutdown();

    bool StartDiscovery(RtpsParticipant* participant);
    void StopDiscovery();

    bool Join(const RoleAttributes& attr, RoleType role,
              bool need_write = true);

    bool Leave(const RoleAttributes& attr, RoleType role);

    ChangeConnection AddChangeListener(const ChangeFunc& func);
    void RemoveChangeListener(const ChangeConnection& conn);
    
protected:

    bool CreateWriter(RtpsParticipant* participant);
    bool CreateReader(RtpsParticipant* participant);

    void OnRemoteChange(const std::string& msg_str);

    virtual bool Check(const RoleAttributes& attr) = 0;

    void Notify(const ChangeMsg& msg);

    bool IsFromSameProcess(const ChangeMsg& msg);

    std::atomic<bool> is_shutdown_;
    std::atomic<bool> is_discovery_started_;
    int allowed_role_;
    ChangeType change_type_;
    std::string host_name_;
    int process_id_;
    std::string channel_name_;
    std::mutex lock_;

    eprosima::fastrtps::rtps::RTPSWriter* writer_;
    eprosima::fastrtps::rtps::RTPSReader* reader_;

    //fastrtps关于ChangeMsg的回调
    ReaderListener* listener_;
    //以msg作为信号
    ChangeSignal signal_;
};




}
}
}

#endif