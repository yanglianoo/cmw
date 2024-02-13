# Cmw的Transport层设计

## 1. 通信整体机构

`cmw`的`Transport`层设计比较复杂，整体通信流程如下图所示：

**代码**：

![image-20240213142416870](image/image-20240213142416870.png)

**发送方**：

- 在发送方通过`Transport`的`CreateTransmiter`函数可以创建两种类型的`Transmitter`：基于`FastRtps`的和基于`Shm`的，通过`Transmitter`的`Transmit`函数就可以将`Message`消息发送出去
- `Message`为自定义的消息数据类型，通过`cmw`中提供的`serialize`序列化库就可以将自定义的数据类型序列化成一段字节数据流
- 对于`RtpsTransmitter`，会将`Message`序列化成`string`类型的数据，然后传递给`FastRtps`的`writer`发送出去；对于`ShmTransmitter`会将`Message`存储在一个名为`ReadableBlock`的内存块中，然后通过`Shm`的方式去拿到这个`ReadableBlock`共享内存块，从而获取`Message`

**接收方**：

- 接收方通过`Transport`的`CreateReceiver`函数可以创建两种类型的`Receiver`;和`Transmitter`一样，支持基于`FastRtps`和`Shm`的通信方式
- 和发送方的主动发送数据不同，接收方是被动的，只有当监测到有数据发出后，接收方才会去接收数据，并去执行回调函数处理数据。
- 监测数据到来的逻辑则是在`Dispatcher`中实现，当发送方发送数据后，`Dispatcher`监测到数据到来时会去执行`Onmessage`函数，此函数会将数据发序列化成`Message`类型，然后会调用`Receiver`的`OnNewMessage`，在`OnNewMessage`函数中会去执行数据接收方注册的回调函数

<img src="image/cmw通信软件架构.png" alt="cmw通信软件架构" style="zoom: 50%;" />

**代码示例**：测试用例在`cmw/example`目录下

- ```c++
  // test_writer.c
  // 使用SHM的方式发送ChangeMsg类型的数据
  #include <cmw/config/topology_change.h>
  #include <cmw/time/time.h>
  void TEST_ChangeMsg()
  {
      using namespace hnu::cmw::config;
      using namespace hnu::cmw;
      using namespace hnu::cmw::common;
      using namespace hnu::cmw::transport;
      std::cout <<"---------------------Transport Transmitter Test---------------------" << std::endl;
      //填充发送端相关配置信息
      RoleAttributes attr;
      attr.channel_name = "exampleTopic";  //发送的channel_name
      attr.host_name = GlobalData::Instance()->HostName();
      attr.host_ip = GlobalData::Instance()->HostIp();
      attr.process_id =  GlobalData::Instance()->ProcessId();
      attr.channel_id = GlobalData::Instance()->RegisterChannel(attr.channel_name);
      QosProfile qos;
      attr.qos_profile = qos;
  	//填充要发送的数据
      ChangeMsg change_msg;
      change_msg.timestamp = Time::Now().ToNanosecond();
      std::cout << "time: " << change_msg.timestamp << std::endl;
      change_msg.change_type = CHANGE_NODE;
      change_msg.operate_type = OPT_JOIN;
      change_msg.role_type = ROLE_WRITER;
      change_msg.role_attr = attr;
  	//创建transmitter
      auto transmitter = Transport::Instance()->CreateTransmitter<ChangeMsg>(attr,OptionalMode::SHM);
  	//创建ChangeMsg的数据指针
      std::shared_ptr<ChangeMsg> msg_ptr = std::make_shared<ChangeMsg>(change_msg);
      //创建MessageInfo
      MessageInfo msg;
      uint64_t n = 0;
      while (1)
      {
          msg.set_seq_num(n);
          std::cout<<"seq: " << n << std::endl;
          n++;
          //发送数据
          transmitter->Transmit(msg_ptr, msg);
          std::this_thread::sleep_for(std::chrono::milliseconds(250));
      }
  }
  int main()
  {
      Logger_Init("writer.log");
      TEST_ChangeMsg();
      return 0;
  }
  ```

- ```c++
  //test_reader.c
  //使用SHM的方式接收ChangeMsg
  #include <cmw/common/global_data.h>
  #include <cmw/config/RoleAttributes.h>
  #include <cmw/transport/dispatcher/rtps_dispatcher.h>
  #include <cmw/common/util.h>
  #include <cmw/transport/receiver/receiver.h>
  #include <cmw/transport/transport.h>
  void TEST_ChangeMsg()
  {
      //填充接收端的配置信息
      RoleAttributes attr;
      attr.channel_name = "exampleTopic";
      attr.host_name = GlobalData::Instance()->HostName();
      attr.host_ip = GlobalData::Instance()->HostIp();
      attr.process_id =  GlobalData::Instance()->ProcessId();
      attr.channel_id = GlobalData::Instance()->RegisterChannel(attr.channel_name);
      QosProfile qos;
      attr.qos_profile = qos;
  	//创建回调函数
      auto listener1 = [](const std::shared_ptr<ChangeMsg>& message ,
                         const MessageInfo& info, const RoleAttributes&){
                          std::cout<<"time: " << message->timestamp << "operate_type:"  << message->operate_type << "seq:" << info.seq_num() << std::endl;  
                         };
      //创建receiver
      auto shm_receiver=Transport::Instance()->CreateReceiver<ChangeMsg>(attr,listener1,OptionalMode::SHM);
      printf("Press Enter to stop the Reader.\n");
      std::cin.ignore();
  }   
  
  int main()
  {
      Logger_Init("reader.log");
      TEST_ChangeMsg();
      return 0;
  }
  ```

## 2. 底层通信机制

通信逻辑如下：

- 对于同主机的同一进程而言，由于同一进程的内存是没有相互隔离的，因此直接通过指针，全局变量的方式通信即可，我们不考虑这种情况；

- 对于同主机的不同进程来说，就可以采用共享内存的方式来实现高效的通信
- 对于不同主机间的进程就只有使用`RTPS`来通过网络进行数据传输了

![img](image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FyaWVzanpq,size_16,color_FFFFFF,t_70#pic_center.png)

### 2.1 FastRtps通信

### 2.2 Shm通信

基于`shm`的通信代码目录为：`cmw/transport/shm`

**通信流程**

![cmw共享内存通信](image/cmw共享内存通信.png)

- 在发送方`ShmTransmitter`会根据`channel_id`创建一个`Segment`，这个`Segment`就是读属于`channel_id`的一片共享内存，写数据就是去写这片内存；
- 接收方的`ShmDispatcher`是一个单例，当`Receiver`想要接收某个`channel_id`来的数据，会先向`ShmDispathcer`注册，而`ShmDispathcer`内部有一个`SegmentMap：<channel_id, Segment>`，通过索引`channel_id`就能找到发送进程创建的对应的`Segment`，从而拿到数据

#### 2.2.1 消息内存结构

**`Segment`的内存结构**: 

![image-20240213153112441](image/image-20240213153112441.png)

- 每个`Segment`共享的内容有
  - 一个`State` 
  - `n`个`Block`
  - `n`个`buffer`

- 一个`Segment`的大小由`msg`的大小来决定，具体的规则定义在：`cmw/transport/shm/shm_conf`中，具体规则如下：比如如果传递的一条`msg`大小在`10k-100k`，则`Segment`的`Block`个数为`128`，一个`msg`的大小为`1024 * 128 = 128k`，`buf`的个数也为`128`。而一个`buf`由两部分组成：

  <img src="image/image-20240213201444395.png" alt="image-20240213201444395" style="zoom:50%;" />

  `MessageInfo`的大小固定为1024个字节，因此此时一个`buf`的大小就是`128k + 1k = 129k`，发送端就往这个`Segment`中写数据，一条数据占据一个`buf`，相当于一个写的队列，当超出队列长度后又会重头开始写。

  `Segment`创建共享内存时的大小为，`EXTRA_SIZE`就是上面灰色的未使用的部分

  ```c++
  // 4K + 1K + 1K + (1K + 129k) * 128
  EXTRA_SIZE+ STATE_SIZE + (BLOCK_SIZE + block_buf_size_) * block_num_
  ```

  ```c++
    // Extra size, Byte
     const uint64_t ShmConf::EXTRA_SIZE = 1024 * 4;
    // State size, Byte
     const uint64_t ShmConf::STATE_SIZE = 1024;
    // Block size, Byte
     const uint64_t ShmConf::BLOCK_SIZE = 1024;
    // Message info size, Byte
     const uint64_t ShmConf::MESSAGE_INFO_SIZE = 1024;
    // For message 0-10K
     const uint32_t ShmConf::BLOCK_NUM_16K = 512;
     const uint64_t ShmConf::MESSAGE_SIZE_16K = 1024 * 16;
    // For message 10K-100K
     const uint32_t ShmConf::BLOCK_NUM_128K = 128;
     const uint64_t ShmConf::MESSAGE_SIZE_128K = 1024 * 128;
    // For message 100K-1M
     const uint32_t ShmConf::BLOCK_NUM_1M = 64;
     const uint64_t ShmConf::MESSAGE_SIZE_1M = 1024 * 1024;
    // For message 1M-6M
     const uint32_t ShmConf::BLOCK_NUM_8M = 32;
     const uint64_t ShmConf::MESSAGE_SIZE_8M = 1024 * 1024 * 8;
    // For message 6M-10M
     const uint32_t ShmConf::BLOCK_NUM_16M = 16;
     const uint64_t ShmConf::MESSAGE_SIZE_16M = 1024 * 1024 *16;
    // For message 10M+
     const uint32_t ShmConf::BLOCK_NUM_MORE = 8;
     const uint64_t ShmConf::MESSAGE_SIZE_MORE = 1024 * 1024 * 32;
  ```

- `State`的定义：`cmw/transport/shm/state.h`

  - 一个`State`包含四个原子变量，所以他们都是进程安全的
  - `need_remap`代表这个`Segment`是否需要重新映射
  - `ceiling_msg_size`代表`msg`的大小即一个`buf`的字节数，当然一个`buf`的大小根据上面的规则肯定是`>= `一条消息的大小，从上面的规则可以看出，`msg`越大，队列长度越短。
  - `seq_`代表的是当前正在写的`block`的索引，发送端每发送一条消息就会去写入一个`Block`和一个`buf`，每次写完数据后就会将`seq+1`
  - `reference_count`代表使用这片内存的用户的个数

  <img src="image/image-20240213155045751.png" alt="image-20240213155045751" style="zoom:67%;" />

- `Block`的定义：`cmw/transport/shm/block.h`

  - `lock_num_`是一个原子变量，此原子变量用于控制`Block`对应的那个`buf`的读写互斥，用于做到进程间的安全读写
  - `msg_size`__和`msg_info_size_`就是消息的长度了，不用多说
  
  <img src="image/image-20240213161708808.png" alt="image-20240213161708808" style="zoom:67%;" />

> 共享内存的创建有在`Linux`下有两种方式，一种是基于`posix`标准的，一种是基于`System v`，因此有两种方式来创建一个`Segment`，分别对应`shm/posix_segment`和`shm/xsi_segment`，默认创建方式为`xsi_segment`

**写入数据**：

- 当`Transmitter`需要发送信息时，先会创建一个`Segment`，然后从此`Segment`中取出一个可以写的`WritableBlock`，然后写入数据；`WritableBlock`定义如下：

  ```c++
  //可写的块内存结构体
  struct WritableBlock
  {
      uint32_t index = 0;
      Block* block = nullptr;
      uint8_t* buf = nullptr;
  };
  ```

  一个`Block`对应一个`buf`，`Block`描述了对应的`buf`的信息，将`Block`和`buf`组成一对再加上索引`index`就构成了一个`WritableBlock`；如果要写入`msg`大小超过当前`Block`的`ShmConf::MESSAGE_SIZE_n`，会触发`remap`，即让当前`Segment`重新创建更大的内存，其他进程使用此`Segment`时依然再读取旧的共享内存的`State`，会触发`remap`，然后会重新打开创建的更大的内存。实际上`Segment`被创建时默认使用最小的`ShmConf::MESSAGE_SIZE_16K`，所以如果大于这个的`msg`在第一次写入时都触发`remap`;

#### 2.2.2 消息通知机制



## 3. 发送方的设计

### 3.1 基于FastRtps的Transmitter

### 3.2 基于Shm的Receiver

## 4.接收方的设计

### 4.1 `ListenerHandler`

