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

    virtual bool Init() = 0;

    virtual void Shutdown() = 0;

    virtual bool HasSubscriber() { return false; }

    virtual void GetSubscriber(std::vector<RoleAttributes>* readers) {}

    const std::string& GetChannelName() const {
        return role_attr_.channel_name;
    }

    bool IsInit() const  {return init_.load();}


protected:
    RoleAttributes role_attr_;
    std::atomic<bool> init_;
};


}
}
#endif