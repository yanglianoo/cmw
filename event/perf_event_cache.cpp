#include <cmw/event/perf_event_cache.h>

namespace hnu    {
namespace cmw   {
namespace event {

PerfEventCache::PerfEventCache(){
    // 需要根据配置文件读取 enable_ 的值用于标识是否开启事件记录功能

    if(enable_){
        if(!event_queue_.Init(kEventQueueSize)){
            std::cout << "Event queue init failed." << std::endl;
            throw std::runtime_error("Event queue init failed.");
        }
        // 开启一个异步线程用于管理事件队列
        Start();
    }
}

//析构时会去调用Shutdown()函数
PerfEventCache::~PerfEventCache() { Shutdown(); }



void PerfEventCache::AddTransportEvent(const TransPerf event_id,
                                       const uint64_t channel_id,
                                       const uint64_t msg_seq,
                                       const uint64_t stamp,
                                       const std::string& adder) 
{
    if(!enable_)
    {
        return;
    }

    EventBasePtr  e = std::make_shared<TransportEvent>();

    e->set_eid(static_cast<int>(event_id));
    e->set_channel_id(channel_id);
    e->set_msg_seq(msg_seq);
    e->set_adder(adder);
    e->set_stamp(stamp);
    event_queue_.Enqueue(e);
                          
}
void PerfEventCache::Run(){
    
    EventBasePtr event;
    // 记录文件数据刷新的的cache buf大小
    int buf_size = 0;
    while (!shutdown_)
    {
        //等待事件队列中有事件产生
        if(event_queue_.WaitDequeue(&event)){
            of_ << event->SerializeToString() << std::endl;
            buf_size++;
            if(buf_size >= kFlushSize){
                //一次性写入文件
                of_.flush();
                buf_size = 0 ;
            }
        }
    }
    
}

void PerfEventCache::Start(){
    

    std::string perf_file = "cmw_perf_.data";
    
    //将文件名中的 ' ' 替换成 '_' ， ':' 替换成 '-'
    std::replace(perf_file.begin(), perf_file.end(), ' ', '_');
    std::replace(perf_file.begin(), perf_file.end(), ':', '-');

    //打开文件
    of_.open(perf_file, std::ios::trunc);

    perf_file_ = perf_file;

    io_thread_ = std::thread(&PerfEventCache::Run, this);

}
}
}
}

