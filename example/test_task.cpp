#include <cmw/task/task.h>
#include <gtest/gtest.h>
#include <cmw/init.h>

using namespace hnu::cmw;

class Foo {
 public:
  void RunOnce() {
    auto res = Async(&Foo::Task, this, 10);
    EXPECT_EQ(res.get(), 10);
  }

  uint32_t Task(const uint32_t& input) { return input; }
};

struct Message {
  uint64_t id;
};

void Task1() { ADEBUG << "Task1 running"; }

void Task2(const Message& input) {
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ADEBUG << "Task2 running";
}

uint64_t Task3(const std::shared_ptr<Message>& input) {
  ADEBUG << "Task3 running";
  return input->id;
}

void test_create_task(){
    auto task_1 = Async(&Task1);
    task_1.get();

    Message msg;
    auto task_2 = Async(&Task2, msg);
    task_2.get();

    auto shared_msg = std::make_shared<Message>();
    shared_msg->id = 1;
    auto task_3 = Async(&Task3, shared_msg);
    EXPECT_EQ(task_3.get(), 1);
}

void test_batch_run(){
    std::vector<std::future<void>> void_results;
  for (int i = 0; i < 10; i++) {
    void_results.push_back(Async(&Task1));
  }

  for (auto& result : void_results) {
    result.get();
  }

  int loop = 10;
  std::vector<std::future<uint64_t>> int_results;
  for (int i = 0; i < loop; i++) {
    auto shared_msg = std::make_shared<Message>();
    shared_msg->id = i;
    int_results.push_back(Async(&Task3, shared_msg));
  }

  for (int i = 0; i < loop; i++) {
    EXPECT_EQ(int_results[i].get(), i);
  }
}
void test_run_member_function(){
    AINFO << " test_run_member_function ";
    Foo foo;
    foo.RunOnce();
    AINFO << " test_run_member_function DEBUG ";
}

int main()
{
    Init("test_task");

    test_create_task();

    test_batch_run();

    test_run_member_function();

    return 0;
}
