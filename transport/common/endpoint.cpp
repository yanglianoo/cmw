#include <cmw/transport/common/endpoint.h>
#include <cmw/common/global_data.h>



namespace hnu    {
namespace cmw   {
namespace transport {


Endpoint::Endpoint(const RoleAttributes& attr) : enabled_(false), id_() , attr_(attr){

    if(!attr_.host_name.empty()){
        attr_.host_name = common::GlobalData::Instance()->HostName();
    }

    if(!attr.process_id){
        attr_.process_id = common::GlobalData::Instance()->ProcessId();
    }

    if(!attr_.id){
        attr_.id = id_.HashValue();
    }

}

Endpoint::~Endpoint() {}

}
}
}