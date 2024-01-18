#include <cmw/discovery/specific_manager/channel_manager.h>
#include <cmw/base/macros.h>

namespace hnu {
namespace cmw {
namespace discovery{ 


ChannelManager::ChannelManager() {
    allowed_role_ |= 1 << RoleType::ROLE_WRITER;
    allowed_role_ |= 1 << RoleType::ROLE_READER;
    change_type_ = ChangeType::CHANGE_CHANNEL;
    channel_name_ = "channel_change_broadcast";
}


ChannelManager::~ChannelManager() {}

bool ChannelManager::Check(const RoleAttributes& attr){
    RETURN_VAL_IF(attr.channel_name.empty(),false);
    RETURN_VAL_IF(!attr.channel_id, false);
    RETURN_VAL_IF(!attr.id, false);
}


}
}
}