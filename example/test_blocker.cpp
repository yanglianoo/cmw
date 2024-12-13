#include <cmw/blocker/blocker.h>
#include <gtest/gtest.h>
#include <cmw/config/unit_test.h>
#include <cmw/blocker/blocker_manager.h>
#include <cmw/common/macros.h>

using namespace hnu::cmw::config;
using namespace hnu::cmw::blocker;


void BlockerTest_constructor(){
    BlockerAttr attr(10, "channel");
    Blocker<UnitTest> blocker(attr);

    EXPECT_EQ(blocker.capacity(), 10);
    EXPECT_EQ(blocker.channel_name(), "channel");

    blocker.set_capacity(20);
    EXPECT_EQ(blocker.capacity(), 20);
}

void BlockerTest_setcapacity(){
    BlockerAttr attr(0, "channel");
    Blocker<UnitTest> blocker(attr);
    EXPECT_EQ(blocker.capacity(), 0);

    UnitTest msg;
    msg.class_name="BlockerTest";
    msg.case_name="publish";
    blocker.Publish(msg);

    blocker.set_capacity(2);
    EXPECT_EQ(blocker.capacity(), 2);

    blocker.Publish(msg);
    blocker.Publish(msg);
    blocker.Publish(msg);
    blocker.set_capacity(1);
    EXPECT_EQ(blocker.capacity(), 1);
}

void BlockerTest_publish(){
    BlockerAttr attr(10, "channel");
    Blocker<UnitTest> blocker(attr);

    auto msg1 = std::make_shared<UnitTest>();
    msg1->class_name = "BlockerTest";
    msg1->case_name = "publish_1";

    UnitTest msg2;
    msg2.class_name = "BlockerTest";
    msg2.case_name = "publish_2";

    EXPECT_TRUE(blocker.IsPublishedEmpty());

    blocker.Publish(msg1);
    blocker.Publish(msg2);
    EXPECT_FALSE(blocker.IsPublishedEmpty());

    EXPECT_TRUE(blocker.IsObservedEmpty());
    blocker.Observe();
    EXPECT_FALSE(blocker.IsObservedEmpty());

    auto& latest_observed_msg = blocker.GetLatestObserved();
    EXPECT_EQ(latest_observed_msg.class_name, "BlockerTest");
    EXPECT_EQ(latest_observed_msg.case_name, "publish_2");

    auto latest_observed_msg_ptr = blocker.GetLatestObservedPtr();
    EXPECT_EQ(latest_observed_msg_ptr->class_name, "BlockerTest");
    EXPECT_EQ(latest_observed_msg_ptr->case_name, "publish_2");

    auto latest_published_ptr = blocker.GetLatestPublishedPtr();
    EXPECT_EQ(latest_published_ptr->class_name, "BlockerTest");
    EXPECT_EQ(latest_published_ptr->case_name, "publish_2");

    blocker.ClearPublished();
    blocker.ClearObserved();
    EXPECT_TRUE(blocker.IsPublishedEmpty());
    EXPECT_TRUE(blocker.IsObservedEmpty());
}

void BlockerTest_subscribe(){
    BlockerAttr attr(10, "channel");
    Blocker<UnitTest> blocker(attr);

    auto received_msg = std::make_shared<UnitTest>();
    bool res = blocker.Subscribe(
      "BlockerTest1", [&received_msg](const std::shared_ptr<UnitTest>& msg) {
        *received_msg = *msg;
      });
    EXPECT_TRUE(res);

    auto msg1 = std::make_shared<UnitTest>();
    msg1->class_name = "BlockerTest";
    msg1->case_name = "publish_1";

    blocker.Publish(msg1);

    EXPECT_EQ(received_msg->class_name, msg1->class_name);
    EXPECT_EQ(received_msg->case_name, msg1->case_name);

    res = blocker.Subscribe(
      "BlockerTest1", [&received_msg](const std::shared_ptr<UnitTest>& msg) {
        *received_msg = *msg;
      });

    EXPECT_FALSE(res);
}


void cb(const std::shared_ptr<UnitTest>& msg_ptr) { UNUSED(msg_ptr); }

void BlockerTest_blocker_manager_test(){
    auto block_mgr = BlockerManager::Instance();
    Blocker<UnitTest>::MessageType msgtype;

    block_mgr->Publish<UnitTest>("ch1", msgtype);
    block_mgr->Subscribe<UnitTest>("ch1", 10, "cb1", cb);
    auto blocker = block_mgr->GetOrCreateBlocker<UnitTest>(BlockerAttr("ch1"));
    EXPECT_NE(blocker, nullptr);
    block_mgr->Unsubscribe<UnitTest>("ch1", "cb1");
    block_mgr->Subscribe<UnitTest>("ch_null", 10, "cb1", cb);
    block_mgr->Observe();
    block_mgr->Reset();  
}

int main()
{
    BlockerTest_constructor();
    BlockerTest_setcapacity();
    BlockerTest_publish();
    BlockerTest_subscribe();
    BlockerTest_blocker_manager_test();
    return 0;
}
