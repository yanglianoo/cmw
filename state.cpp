#include <cmw/state.h>


#include <atomic>

namespace hnu {
namespace cmw {

namespace {
std::atomic<State> g_cmw_state;
}

State GetState() { return g_cmw_state.load(); }

void SetState(const State& state) { g_cmw_state.store(state); }
}
}