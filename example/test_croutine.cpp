#include <cmw/croutine/croutine.h>
#include <cmw/common/global_data.h>
#include <cmw/init.h>
#include <gtest/gtest.h>

using namespace hnu::cmw::croutine;
using namespace hnu::cmw::common;

void function() { CRoutine::Yield(RoutineState::IO_WAIT); }

int main()
{
    hnu::cmw::Init("croutine_test");
    std::shared_ptr<CRoutine> cr = std::make_shared<CRoutine>(function);

    auto id = GlobalData::RegisterTaskName("croutine");

    cr->set_id(id);
    cr->set_name("croutine");
    cr->set_processor_id(0);
    cr->set_priority(1);
    cr->set_state(RoutineState::DATA_WAIT);

    EXPECT_EQ(cr->state(), RoutineState::DATA_WAIT);
    cr->Wake();
    EXPECT_EQ(cr->state(), RoutineState::READY);

    cr->UpdateState();
    EXPECT_EQ(cr->state(), RoutineState::READY);
    EXPECT_EQ(*(cr->GetMainStack()), nullptr);
    cr->Resume();
    EXPECT_NE(*(cr->GetMainStack()), nullptr);
    EXPECT_EQ(cr->state(), RoutineState::IO_WAIT);
    cr->Stop();
    EXPECT_EQ(cr->Resume(), RoutineState::FINISHED);
    return 0;
}