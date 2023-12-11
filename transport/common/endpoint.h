#ifndef CMW_TRANSPORT_COMMON_ENDPOINT_H_
#define CMW_TRANSPORT_COMMON_ENDPOINT_H_


#include <cmw/transport/common/identity.h>
#include <cmw/config/RoleAttributes.h>
#include <memory>

namespace hnu    {
namespace cmw   {
namespace transport {

using namespace config;
class Endpoint;
using EndpointPtr = std::shared_ptr<Endpoint>;

class Endpoint
{
    public:
        explicit Endpoint(const RoleAttributes& attr);
        virtual ~Endpoint();
    
        const Identity& id() const{ return id_;}
        const RoleAttributes& attributes() const { return attr_;}

    private:
        bool enabled_;
        Identity id_;
        RoleAttributes attr_;

};

}
}
}

#endif