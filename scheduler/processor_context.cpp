#include <cmw/scheduler/processor_context.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {


void ProcessorContext::Shutdown() { stop_.store(true); }
}
}
}