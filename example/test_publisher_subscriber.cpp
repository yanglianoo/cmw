#include <string>
#include <gtest/gtest.h>
#include <cmw/init.h>
#include <cmw/node/subscriber.h>
#include <cmw/node/publisher.h>
#include <cmw/config/unit_test.h>

using namespace hnu::cmw;

void WriterReaderTest_constructor(){
    const std::string channel_name("constructor");
    RoleAttributes attr;
    attr.channel_name = channel_name;

    Publisher<config::UnitTest> publisher_a(attr);
    EXPECT_FALSE(publisher_a.IsInit());
    EXPECT_EQ(publisher_a.GetChannelName(), channel_name);

    Subscriber<config::UnitTest> subscriber_a(attr);
    EXPECT_FALSE(subscriber_a.IsInit());
    EXPECT_EQ(subscriber_a.GetChannelName(), channel_name);    

    attr.host_name = "caros";
    Publisher<config::UnitTest> publisher_b(attr);
    EXPECT_FALSE(publisher_b.IsInit());
    EXPECT_EQ(publisher_b.GetChannelName(), channel_name);

    Subscriber<config::UnitTest> subscriber_b(attr);
    EXPECT_FALSE(subscriber_b.IsInit());
    EXPECT_EQ(subscriber_b.GetChannelName(), channel_name);  

    attr.process_id = 12345;
    Publisher<config::UnitTest> publisher_c(attr);
    EXPECT_FALSE(publisher_c.IsInit());
    EXPECT_EQ(publisher_c.GetChannelName(), channel_name);

    Subscriber<config::UnitTest> subscriber_c(attr);
    EXPECT_FALSE(subscriber_c.IsInit());
    EXPECT_EQ(subscriber_c.GetChannelName(), channel_name);  
}

void WriterReaderTest_init_and_shutdown(){
    const std::string channel_name_a("init");
    const std::string channel_name_b("shutdown");  

    RoleAttributes attr;
    attr.channel_name = channel_name_a;
    attr.host_name = "caros";
    attr.process_id = 12345;

    Publisher<config::UnitTest> publisher_a(attr);
    EXPECT_TRUE(publisher_a.Init());
    EXPECT_TRUE(publisher_a.IsInit());   
    EXPECT_TRUE(publisher_a.Init());

    attr.channel_name = channel_name_b;
    attr.process_id = 54321;
    Subscriber<config::UnitTest> subscriber_a(attr);
    EXPECT_TRUE(subscriber_a.Init());
    EXPECT_TRUE(subscriber_a.IsInit());
    // repeated call
    EXPECT_TRUE(subscriber_a.Init());

    Publisher<config::UnitTest> publisher_b(attr);
    EXPECT_TRUE(publisher_b.Init());
    EXPECT_TRUE(publisher_b.IsInit());   

    attr.channel_name = channel_name_a;
    attr.host_name = "sorac";
    attr.process_id = 12345;

    Subscriber<config::UnitTest> subscriber_b(attr);
    EXPECT_TRUE(subscriber_b.Init());
    EXPECT_TRUE(subscriber_b.IsInit());

    //用相同的attr去创建另外一个Subscriber
    Subscriber<config::UnitTest> subscriber_c(attr);
    EXPECT_FALSE(subscriber_c.Init());
    EXPECT_FALSE(subscriber_c.IsInit());

    publisher_a.Shutdown();

    publisher_a.Shutdown();
    subscriber_a.Shutdown();

    subscriber_a.Shutdown();
    publisher_b.Shutdown();
    subscriber_b.Shutdown();
    subscriber_c.Shutdown();

    EXPECT_FALSE(publisher_a.IsInit());
    EXPECT_FALSE(publisher_b.IsInit());
    EXPECT_FALSE(subscriber_a.IsInit());
    EXPECT_FALSE(subscriber_b.IsInit());
    EXPECT_FALSE(subscriber_c.IsInit());
}

int main()
{
    hnu::cmw::Init("Test_publisher_subscriber");
    WriterReaderTest_constructor();
    WriterReaderTest_init_and_shutdown();
    return 0;
}
