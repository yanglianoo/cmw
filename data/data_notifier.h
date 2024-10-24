#ifndef CMW_DATA_DATA_NOTIFIER_H_
#define CMW_DATA_DATA_NOTIFIER_H_

#include <memory>
#include <mutex>
#include <vector>

#include <cmw/common/log.h>
#include <cmw/common/macros.h>
#include <cmw/data/cache_buffer.h>
#include <cmw/time/time.h>
#include <cmw/base/atmoic_hash_map.h>

namespace hnu {
namespace cmw {
namespace data{

using hnu::cmw::Time;
using hnu::cmw::base::AtomicHashMap;


struct Notifier {
    std::function<void()> callback;
};

class DataNotifier{
 public:
    using NotifyVector = std::vector<std::shared_ptr<Notifier>>;
    ~DataNotifier() {}

    void AddNotifier(uint64_t channel_id,
                     const std::shared_ptr<Notifier>& notifier);
    bool Notify(const uint64_t channel_id);
 private:
    std::mutex notifies_map_mutex_;
    AtomicHashMap<uint64_t, NotifyVector> notifies_map_;

    DECLARE_SINGLETON(DataNotifier);
};

inline DataNotifier::DataNotifier() {}

inline void DataNotifier::AddNotifier(uint64_t channel_id,
                     const std::shared_ptr<Notifier>& notifier){
        std::lock_guard<std::mutex> lock(notifies_map_mutex_);
        NotifyVector* notifies = nullptr;
        if(notifies_map_.Get(channel_id, &notifies)){
            notifies->emplace_back(notifier);
        }else{
            NotifyVector new_notify = {notifier};
            notifies_map_.Set(channel_id, new_notify);
        }
}

inline bool DataNotifier::Notify(const uint64_t channel_id) {
  NotifyVector* notifies = nullptr;
  if (notifies_map_.Get(channel_id, &notifies)) {
    for (auto& notifier : *notifies) {
      if (notifier && notifier->callback) {
        notifier->callback();
      }
    }
    return true;
  }
  return false;
}

}
}
}


#endif