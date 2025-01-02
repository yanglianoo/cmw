#ifndef CMW_INIT_H_
#define CMW_INIT_H_

#include <cmw/common/log.h>
#include <cmw/state.h>
#include <cmw/node/node.h>

namespace hnu {
namespace cmw {

bool Init(const char* binary_name);
void Clear();

std::unique_ptr<Node> CreateNode(const std::string& node_name,
                                 const std::string& name_space = "");

}
}
#endif