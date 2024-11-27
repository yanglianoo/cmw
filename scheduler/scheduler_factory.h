#ifndef CMW_SCHEDULER_SCHEDULER_FACTORY_H_
#define CMW_SCHEDULER_SCHEDULER_FACTORY_H_

#include <cmw/common/environment.h>
#include <cmw/common/file.h>
#include <cmw/common/global_data.h>
#include <cmw/common/util.h>
#include <cmw/scheduler/policy/scheduler_choreography.h>
#include <cmw/scheduler/policy/scheduler_classic.h>
#include <cmw/scheduler/scheduler.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

Scheduler* Instance();
void CleanUp();

}
}
}


#endif