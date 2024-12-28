#include <cmw/node/subscriber.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/message/message_info.h>
#include <cmw/config/unit_test.h>
#include <gtest/gtest.h>
#include <cmw/init.h>
using namespace hnu::cmw;

struct TestMsg : public Serializable
{
    uint64_t timestamp;  

    std::vector<char> image;
    SERIALIZE(timestamp,image)
};

namespace george
{
    void test_rtps_sub(void)
    {
        config::RoleAttributes role_attr;
        role_attr.channel_name = "/chatter0";
        role_attr.node_name = "subscriber";
        role_attr.channel_id =common::GlobalData::RegisterChannel("/chatter0");


        Subscriber<TestMsg> subscriber(
                role_attr,[](const std::shared_ptr<TestMsg>& msg){
                    std::cout << "timestamp is "<< msg->timestamp << std::endl;
                });

        std::boolalpha;

        std::cout<<"Init subscriber " << subscriber.Init() << std::endl;

        while (1)
        {
            /* code */
        }
    }
}

namespace Timer{


using namespace config;

auto callback = [](const std::shared_ptr<Chatter>& msg){
    AINFO << "msg content: " << msg->content;
};

void SubscriberTest1(){

    RoleAttributes role;
    {
        Subscriber<Chatter> r(role,callback,0);
        EXPECT_EQ(r.PendingQueueSize(), 0);
        EXPECT_TRUE(r.GetChannelName().empty());
        EXPECT_TRUE(r.Init());
        EXPECT_TRUE(r.GetChannelName().empty());
        EXPECT_TRUE(r.IsInit());

        r.Observe();

        EXPECT_FALSE(r.HasReceived());
        EXPECT_TRUE(r.Empty());
        EXPECT_LT(r.GetDelaySec(), 0);

        r.ClearData();
        r.Shutdown();
    }

    {
        Subscriber<Chatter> r(role,callback,100);
        EXPECT_EQ(r.PendingQueueSize(), 100);
        EXPECT_TRUE(r.GetChannelName().empty());
        EXPECT_TRUE(r.Init());
        EXPECT_TRUE(r.GetChannelName().empty());
        EXPECT_TRUE(r.IsInit());

        r.Observe();

        EXPECT_FALSE(r.HasReceived());
        EXPECT_TRUE(r.Empty());
        EXPECT_LT(r.GetDelaySec(), 0);
        r.ClearData();
        r.Shutdown();
    }

    auto qos = &role.qos_profile;
    qos->history = QosHistoryPolicy::HISTORY_KEEP_LAST;
    qos->depth = 1;
    qos->mps = 0;
    qos->reliability = QosReliabilityPolicy::RELIABILITY_RELIABLE;
    qos->durability = QosDurabilityPolicy::DURABILITY_VOLATILE;
    role.channel_name = "/chatter0";

    {
        Subscriber<Chatter> r(role,callback,100);
        EXPECT_EQ(r.PendingQueueSize(), 100);
        EXPECT_EQ(r.GetChannelName(), "/chatter0");
        EXPECT_TRUE(r.Init());
        EXPECT_EQ(r.GetChannelName(), "/chatter0");
        EXPECT_TRUE(r.IsInit());

        r.Observe();

        EXPECT_FALSE(r.HasReceived());
        EXPECT_TRUE(r.Empty());
        EXPECT_LT(r.GetDelaySec(), 0);

        r.ClearData();
        r.Shutdown();
    }

    AINFO << "Finish SubscriberTest1";
}

void SubscriberTest2(){
    RoleAttributes role;
    auto qos = &role.qos_profile;
    qos->history = QosHistoryPolicy::HISTORY_KEEP_LAST;
    qos->depth = 0;
    qos->mps = 0;
    qos->reliability = QosReliabilityPolicy::RELIABILITY_RELIABLE;
    qos->durability = QosDurabilityPolicy::DURABILITY_VOLATILE;
    role.channel_name = "/chatter0";

    Subscriber<Chatter> r(role,callback,100);
    EXPECT_EQ(r.PendingQueueSize(), 100);
    EXPECT_EQ(r.GetChannelName(), "/chatter0");
    EXPECT_TRUE(r.Init());
    r.SetHistoryDepth(1);
    EXPECT_TRUE(r.Init());
    {
        auto c = std::make_shared<Chatter>();
        c->timestamp = Time::Now().ToNanosecond();
        c->lidar_timestamp = Time::Now().ToNanosecond();
        c->seq = 1;
        c->content = "ChatterMsg";
        r.Enqueue(c);
    }

    EXPECT_EQ(r.GetHistoryDepth(), 1);
    EXPECT_GT(r.GetDelaySec(), 0);

    r.SetHistoryDepth(0);
    {
        auto c = std::make_shared<Chatter>();
        c->timestamp = Time::Now().ToNanosecond();
        c->lidar_timestamp = Time::Now().ToNanosecond();
        c->seq = 2;
        c->content = "ChatterMsg";
        r.Enqueue(c);
    }

    EXPECT_EQ(r.GetHistoryDepth(), 0);
    EXPECT_EQ(r.Begin(), r.End());
    EXPECT_GT(r.GetDelaySec(), 0);

    r.SetHistoryDepth(3);
    {
        auto c = std::make_shared<Chatter>();
        c->timestamp = Time::Now().ToNanosecond();
        c->lidar_timestamp = Time::Now().ToNanosecond();
        c->seq = 3;
        c->content = "ChatterMsg";
        r.Enqueue(c);
    }
    {
        auto c = std::make_shared<Chatter>();
        c->timestamp = Time::Now().ToNanosecond();
        c->lidar_timestamp = Time::Now().ToNanosecond();
        c->seq = 4;
        c->content = "ChatterMsg";
        r.Enqueue(c);
    }
    {
        auto c = std::make_shared<Chatter>();
        c->timestamp = Time::Now().ToNanosecond();
        c->lidar_timestamp = Time::Now().ToNanosecond();
        c->seq = 5;
        c->content = "ChatterMsg";
        r.Enqueue(c);
    }

    EXPECT_EQ(r.GetHistoryDepth(), 3);

    auto latestMsg = r.GetLatestObserved();
    auto oldestMsg = r.GetOldestObserved();

    EXPECT_EQ(nullptr, latestMsg);
    EXPECT_EQ(nullptr, oldestMsg);
    EXPECT_EQ(r.Begin(), r.End());

    r.Observe();
    latestMsg = r.GetLatestObserved();
    oldestMsg = r.GetOldestObserved();

    EXPECT_EQ(latestMsg->seq, 5);
    EXPECT_EQ(oldestMsg->seq, 3);

    AINFO << "Finish SubscriberTest2";
}
}

int main()
{
    //george::test_rtps_sub();
    hnu::cmw::Init("SubscriberTest");
    Timer::SubscriberTest1();
    Timer::SubscriberTest2();
    return 0;
}
