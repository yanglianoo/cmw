#include <cmw/scheduler/scheduler.h>
#include <cmw/config/scheduler_conf.h>
#include <cmw/common/global_data.h>
#include <cmw/init.h>
#include <cmw/scheduler/processor_context.h>
#include <cmw/scheduler/scheduler_factory.h>
#include <gtest/gtest.h>
#include <vector>
#include <cmw/task/task.h>
#include <cmw/base/for_each.h>
#include <cmw/scheduler/processor.h>
#include <cmw/scheduler/policy/classic_context.h>

using namespace hnu::cmw::common;
using namespace hnu::cmw::croutine;

using namespace hnu::cmw;

void scheduler_testfunc() {
    std::cout << "test cheduler func --------------------------------" <<std::endl;
}
void proc() {}

void test_sched_classic(){
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

void test_classic(){
    auto processor = std::make_shared<scheduler::Processor>();
    auto ctx = std::make_shared<scheduler::ClassicContext>();
    processor->BindContext(ctx);
    std::vector<std::future<void>> res;

    // test single routine
    auto future = Async([]() {
        FOR_EACH(i, 0, 20) { hnu::cmw::SleepFor(std::chrono::milliseconds(i)); }
        AINFO << "Finish task: single";
    });
    future.get();

    // test multiple routine
    FOR_EACH(i, 0, 20) {
        res.emplace_back(Async([i]() {
        FOR_EACH(time, 0, 30) { hnu::cmw::SleepFor(std::chrono::milliseconds(i)); }
        }));
        AINFO << "Finish task: " << i;
    };
    for (auto& future : res) {
        future.wait_for(std::chrono::milliseconds(1000));
    }
    res.clear();
    ctx->Shutdown();
    processor->Stop();
}

void test_create_task(){
  GlobalData::Instance()->SetProcessGroup("example_sched_classic");
  auto sched = scheduler::Instance();

  // read example_sched_classic.conf task 'ABC' prio
  std::string croutine_name = "A";

  EXPECT_TRUE(sched->CreateTask(&proc, croutine_name));
  //create a croutine with the same name
  EXPECT_FALSE(sched->CreateTask(&proc, croutine_name));

  auto task_id = GlobalData::RegisterTaskName(croutine_name);
  EXPECT_TRUE(sched->NotifyTask(task_id));

  EXPECT_TRUE(sched->RemoveTask(croutine_name));
  // remove the same task twice
  EXPECT_FALSE(sched->RemoveTask(croutine_name));
  // remove a not exist task
  EXPECT_FALSE(sched->RemoveTask("driver"));
}

void test_notify_task(){
    auto sched = scheduler::Instance();
    std::string name = "croutine";
    auto id = GlobalData::RegisterTaskName(name);

    // notify task that the id is not exist
    EXPECT_FALSE(sched->NotifyTask(id));
    EXPECT_TRUE(sched->CreateTask(&proc, name));
    EXPECT_FALSE(sched->NotifyTask(id));
}

void test_set_inner_thread_attr(){
    auto sched = scheduler::Instance();
    std::thread t = std::thread([]() {});
    std::unordered_map<std::string, config::InnerThread> thread_confs;
    config::InnerThread inner_thread;
    inner_thread.cpuset = "0-1";
    inner_thread.policy = "SCHED_FIFO";
    inner_thread.prio = 10;
    thread_confs["inner_thread_test"] = inner_thread;
    sched->SetInnerThreadConfs(thread_confs);
    sched->SetInnerThreadAttr("inner_thread_test", &t);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    if (t.joinable()) {
        t.join();
    }
    sched->Shutdown();
    ADEBUG << "Finish test_set_inner_thread_attr";
}

int main(int argc, char const *argv[])
{   
    Init("test_scheduler");
    
    test_create_task();
    //test_sched_classic();
    // test_create_task();
    // test_notify_task();
    // test_set_inner_thread_attr();
    //test_classic();
}
