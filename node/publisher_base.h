#ifndef CMW_NODE_PUBLISHER_BASE_H_
#define CMW_NODE_PUBLISHER_BASE_H_

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#include <cmw/config/RoleAttributes.h>
namespace hnu    {
namespace cmw   {

using namespace config;
class PublisherBase
{
public:
    explicit PublisherBase(const RoleAttributes& role_attr)
                : role_attr_(role_attr) , init_(false) {}
    virtual ~PublisherBase() {}

    //初始化Publisher
    virtual bool Init() = 0;
    //关闭 Publisher
    virtual void Shutdown() = 0;
    //判断是否有Subscriber订阅了此Publisher
    virtual bool HasSubscriber() { return false; }
    //拿到所有订阅了此Publisher的Subscriber
    virtual void GetSubscribers(std::vector<RoleAttributes>* subscribers) {}
    //拿到此Publisher的Channel name
    const std::string& GetChannelName() const {
        return role_attr_.channel_name;
    }
    //判断此Publisher是否已经初始化了
    bool IsInit() const  {
        std::lock_guard<std::mutex> g(lock_);
        return init_;
    }

protected:
    RoleAttributes role_attr_;
    std::atomic<bool> init_;
    mutable std::mutex lock_;
};


}
}
#endif