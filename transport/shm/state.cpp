
#include <cmw/transport/shm/state.h>

namespace hnu{
namespace cmw{
namespace transport{

State::State(const uint64_t& ceiling_msg_size)
    : ceiling_msg_size_(ceiling_msg_size) {}

State::~State() {}
}
}
}