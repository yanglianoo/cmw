#ifndef CMW_COMMON_GLOBAL_DATA_H_
#define CMW_COMMON_GLOBAL_DATA_H_

#include <cmw/base/atmoic_hash_map.h>
#include <cmw/common/macros.h>
#include <string>
namespace hnu {
namespace cmw {
namespace common {

class GlobalData {
 public:
  ~GlobalData();
  
  //返回当前进程的ID
  int ProcessID() const;
  

  private:


    std::string host_ip_;
    std::string host_name_;

    int porcess_id_;
    std::string process_group_;

    std::string sched_name_ = "HNU_CMW_DEFAULT";    


}

}
}
}





#endif