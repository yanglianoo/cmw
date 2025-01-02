#include <cmw/node/node.h>
#include <cmw/common/global_data.h>
#include <cmw/time/time.h>

namespace hnu    {
namespace cmw   {


Node::Node(const std::string& node_name, const std::string& name_space)
        :node_name_(node_name), name_space_(name_space){
    node_channel_impl_.reset(new NodeChannelImpl(node_name));
}
Node::~Node() {}

const std::string& Node::Name() const { return node_name_; }

void Node::Observe(){
    for(auto& subscriber : subscribers_){
        subscriber.second->Observe();
    }
}

void Node::ClearData() {
    for(auto& subscriber : subscribers_){
        subscriber.second->ClearData();
    }
}

}
}