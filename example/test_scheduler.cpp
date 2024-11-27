#include <cmw/scheduler/scheduler.h>
#include <cmw/config/scheduler_conf.h>
#include <cmw/common/global_data.h>
#include <cmw/init.h>
#include <cmw/scheduler/processor_context.h>
#include <cmw/scheduler/scheduler_factory.h>
#include <gtest/gtest.h>

using namespace hnu::cmw::common;
using namespace hnu::cmw::croutine;

using namespace hnu::cmw;

void scheduler_testfunc() {
    std::cout << "test cheduler func" <<std::endl;
}

int main(int argc, char const *argv[])
{   
    Init("test_scheduler");
    GlobalData::Instance()->SetProcessGroup("example_sched_classic");
    auto sched1 = dynamic_cast<scheduler::SchedulerClassic*>(scheduler::Instance());

    std::shared_ptr<CRoutine> cr = std::make_shared<CRoutine>(scheduler_testfunc);
    
    auto task_id = GlobalData::RegisterTaskName("ABC");
    cr->set_id(task_id);
    cr->set_name("ABC");

    EXPECT_TRUE(sched1->DispatchTask(cr));
    // dispatch the same task
    EXPECT_FALSE(sched1->DispatchTask(cr));
    EXPECT_TRUE(sched1->RemoveTask("ABC"));

    std::shared_ptr<CRoutine> cr1 = std::make_shared<CRoutine>(scheduler_testfunc);

    cr1->set_id(GlobalData::RegisterTaskName("xxxxxx"));
    cr1->set_name("xxxxxx");
    EXPECT_TRUE(sched1->DispatchTask(cr1));

    auto t = std::thread(scheduler_testfunc);
    sched1->SetInnerThreadAttr("shm", &t);
    if (t.joinable()) {
        t.join();
    }
    ADEBUG << "-----------Finished Test_scheduler---------";
}
