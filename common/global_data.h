#ifndef CMW_COMMON_GLOBAL_DATA_H_
#define CMW_COMMON_GLOBAL_DATA_H_

#include <cmw/base/atmoic_hash_map.h>
#include <cmw/common/macros.h>
#include <cmw/base/atomic_rw_lock.h>
#include <string>
namespace hnu {
namespace cmw {
namespace common {

using ::hnu::cmw::base::AtomicHashMap;

class GlobalData {
 public:
  ~GlobalData();

  //返回当前进程的ID
  int ProcessId() const;
  void SetProcessGroup(const std::string& process_group);
  const std::string& ProcessGroup() const;
  const std::string& HostIp() const;
  const std::string& HostName() const;

  static std::string GetChannelById(uint64_t id);
  //根据传入的channel_name 向 channel_id_map_ 中注册id 
  static uint64_t RegisterChannel(const std::string& channel);

  private:
    void InitHostInfo();
    // 运行机器的配置信息
    std::string host_ip_;
    std::string host_name_;

    //当前进程的信息
    int porcess_id_;
    std::string process_group_;

      

    // run mode 暂时没有运行模式
     
    // sched policy info  暂时不支持调度
    std::string sched_name_ = "HNU_CMW_DEFAULT";  

    //在创建新的channel时会注册进此全局map
    static AtomicHashMap<uint64_t, std::string, 256> channel_id_map_;   //全局 channel_id_map_ 表


    //GlobalData为全局单例
    DECLARE_SINGLETON(GlobalData)
    
};

}
}
}





#endif