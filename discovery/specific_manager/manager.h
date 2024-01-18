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

    /**
     * @brief Startup topology discovery
     *
     * @param participant is used to create rtps Writer and Reader
     * @return true if start successfully
     * @return false if start fail
   */
    bool StartDiscovery(RtpsParticipant* participant);

    /**
     * @brief Stop topology discovery
     */
    void StopDiscovery();


    /**
     * @brief Join the topology
     *
     * @param attr is the attributes that will be sent to other Manager(include
     * ourselves)
     * @param role is one of RoleType enum
     * @return true if Join topology successfully
     * @return false if Join topology failed
     */
    bool Join(const RoleAttributes& attr, RoleType role,
              bool need_write = true);

    /**
     * @brief Leave the topology
     *
     * @param attr is the attributes that will be sent to other Manager(include
     * ourselves)
     * @param role if one of RoleType enum.
     * @return true if Leave topology successfully
     * @return false if Leave topology failed
     */   
    bool Leave(const RoleAttributes& attr, RoleType role);

    /**
     * @brief Add topology change listener, when topology changed, func will be
     * called.
     *
     * @param func the callback function
     * @return ChangeConnection Store it to use when you want to stop listening.
     */
    ChangeConnection AddChangeListener(const ChangeFunc& func);

    /**
     * @brief Remove our listener for topology change.
     *
     * @param conn is the return value of `AddChangeListener`
     */
    void RemoveChangeListener(const ChangeConnection& conn);
    
    /**
     * @brief Called when a process' topology manager instance leave
     *
     * @param host_name is the process's host's name
     * @param process_id is the process' id
     */
    virtual void OnTopoModuleLeave(const std::string& host_name,
                                 int process_id) = 0;
protected:

    bool CreateWriter(RtpsParticipant* participant);
    bool CreateReader(RtpsParticipant* participant);

    

    //两个纯虚函数，子类必须实现
    virtual bool Check(const RoleAttributes& attr) = 0;
    virtual void Dispose(const ChangeMsg& msg) = 0;
    virtual bool NeedPublish(const ChangeMsg& msg) const;

    
    void Notify(const ChangeMsg& msg);
    bool Write(const ChangeMsg& msg);
    void OnRemoteChange(const std::string& msg_str);
    bool IsFromSameProcess(const ChangeMsg& msg);

    //填充msg
    void Convert(const RoleAttributes& attr, RoleType role, OperateType opt,
               ChangeMsg* msg);

    std::atomic<bool> is_shutdown_;
    std::atomic<bool> is_discovery_started_;

    //允许的role
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
    //以ChangeMsg作为信号
    ChangeSignal signal_;
};




}
}
}

#endif