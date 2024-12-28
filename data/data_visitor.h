
#ifndef CMW_DATA_DATA_VISITOR_H_
#define CMW_DATA_DATA_VISITOR_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include <cmw/common/log.h>
#include <cmw/data/channel_buffer.h>
#include <cmw/data/data_dispatcher.h>
#include <cmw/data/data_fusion.h>
#include <cmw/data/data_visitor_base.h>
#include <cmw/data/all_latest.h>

namespace hnu {
namespace cmw {
namespace data{

struct VisitorConfig {
    VisitorConfig(uint64_t id, uint32_t size)
        : channel_id(id), queue_size(size) {}
    uint64_t channel_id;
    uint64_t queue_size;
};


template <typename T>
using BufferType = CacheBuffer<std::shared_ptr<T>>;

template <typename M0, typename M1 = NullType, typename M2 = NullType,
          typename M3 = NullType>
class DataVisitor : public DataVisitorBase {
    public:
        explicit DataVisitor(const std::vector<VisitorConfig>& configs)
            : buffer_m0_(configs[0].channel_id, new BufferType<M0>(configs[0].queue_size)),
              buffer_m1_(configs[1].channel_id, new BufferType<M1>(configs[1].queue_size)),
              buffer_m2_(configs[2].channel_id, new BufferType<M2>(configs[2].queue_size)),
              buffer_m3_(configs[3].channel_id, new BufferType<M3>(configs[3].queue_size)) {
            
            DataDispatcher<M0>::Instance()->AddBuffer(buffer_m0_);
            DataDispatcher<M1>::Instance()->AddBuffer(buffer_m1_);
            DataDispatcher<M2>::Instance()->AddBuffer(buffer_m2_);
            DataDispatcher<M3>::Instance()->AddBuffer(buffer_m3_);

            data_notifier_->AddNotifier(buffer_m0_.channel_id(), notifier_);

            data_fusion_ = new fusion::AllLatest<M0, M1, M2, M3>(
                buffer_m0_, buffer_m1_, buffer_m2_, buffer_m3_);
            }

        ~DataVisitor() {
                if (data_fusion_) {
                delete data_fusion_;
                data_fusion_ = nullptr;
            }
        }

        bool TryFetch(std::shared_ptr<M0>& m0, std::shared_ptr<M1>& m1,  // NOLINT
                        std::shared_ptr<M2>& m2) {                         // NOLINT
            if (data_fusion_->Fusion(&next_msg_index_, m0, m1, m2)) {
                next_msg_index_++;
                return true;
            }
            return false;
        }
    private:  
        fusion::DataFusion<M0, M1, M2, M3>* data_fusion_ = nullptr;
        ChannelBuffer<M0> buffer_m0_;
        ChannelBuffer<M1> buffer_m1_;
        ChannelBuffer<M2> buffer_m2_;
        ChannelBuffer<M3> buffer_m3_;
};

template <typename M0, typename M1, typename M2>
class DataVisitor<M0, M1, M2, NullType> : public DataVisitorBase {
 public:
  explicit DataVisitor(const std::vector<VisitorConfig>& configs)
      : buffer_m0_(configs[0].channel_id,
                   new BufferType<M0>(configs[0].queue_size)),
        buffer_m1_(configs[1].channel_id,
                   new BufferType<M1>(configs[1].queue_size)),
        buffer_m2_(configs[2].channel_id,
                   new BufferType<M2>(configs[2].queue_size)) {
    DataDispatcher<M0>::Instance()->AddBuffer(buffer_m0_);
    DataDispatcher<M1>::Instance()->AddBuffer(buffer_m1_);
    DataDispatcher<M2>::Instance()->AddBuffer(buffer_m2_);
    data_notifier_->AddNotifier(buffer_m0_.channel_id(), notifier_);
    data_fusion_ =
        new fusion::AllLatest<M0, M1, M2>(buffer_m0_, buffer_m1_, buffer_m2_);
  }

  ~DataVisitor() {
    if (data_fusion_) {
      delete data_fusion_;
      data_fusion_ = nullptr;
    }
  }

  bool TryFetch(std::shared_ptr<M0>& m0, std::shared_ptr<M1>& m1,  // NOLINT
                std::shared_ptr<M2>& m2) {                         // NOLINT
    if (data_fusion_->Fusion(&next_msg_index_, m0, m1, m2)) {
      next_msg_index_++;
      return true;
    }
    return false;
  }

 private:
  fusion::DataFusion<M0, M1, M2>* data_fusion_ = nullptr;
  ChannelBuffer<M0> buffer_m0_;
  ChannelBuffer<M1> buffer_m1_;
  ChannelBuffer<M2> buffer_m2_;
};

template <typename M0, typename M1>
class DataVisitor<M0, M1, NullType, NullType> : public DataVisitorBase {
 public:
  explicit DataVisitor(const std::vector<VisitorConfig>& configs)
      : buffer_m0_(configs[0].channel_id,
                   new BufferType<M0>(configs[0].queue_size)),
        buffer_m1_(configs[1].channel_id,
                   new BufferType<M1>(configs[1].queue_size)) {
    DataDispatcher<M0>::Instance()->AddBuffer(buffer_m0_);
    DataDispatcher<M1>::Instance()->AddBuffer(buffer_m1_);
    data_notifier_->AddNotifier(buffer_m0_.channel_id(), notifier_);
    data_fusion_ = new fusion::AllLatest<M0, M1>(buffer_m0_, buffer_m1_);
  }

  ~DataVisitor() {
    if (data_fusion_) {
      delete data_fusion_;
      data_fusion_ = nullptr;
    }
  }

  bool TryFetch(std::shared_ptr<M0>& m0, std::shared_ptr<M1>& m1) {  // NOLINT
    if (data_fusion_->Fusion(&next_msg_index_, m0, m1)) {
      next_msg_index_++;
      return true;
    }
    return false;
  }

 private:
  fusion::DataFusion<M0, M1>* data_fusion_ = nullptr;
  ChannelBuffer<M0> buffer_m0_;
  ChannelBuffer<M1> buffer_m1_;
};

template <typename M0>
class DataVisitor<M0, NullType, NullType, NullType> : public DataVisitorBase {
 public:
  explicit DataVisitor(const VisitorConfig& configs)
      : buffer_(configs.channel_id, new BufferType<M0>(configs.queue_size)) {
    DataDispatcher<M0>::Instance()->AddBuffer(buffer_);
    data_notifier_->AddNotifier(buffer_.channel_id(), notifier_);
  }

  DataVisitor(uint64_t channel_id, uint32_t queue_size)
      : buffer_(channel_id, new BufferType<M0>(queue_size)) {
    DataDispatcher<M0>::Instance()->AddBuffer(buffer_);
    data_notifier_->AddNotifier(buffer_.channel_id(), notifier_);
  }

  bool TryFetch(std::shared_ptr<M0>& m0) {  // NOLINT
    if (buffer_.Fetch(&next_msg_index_, m0)) {
      next_msg_index_++;
      return true;
    }
    return false;
  }

 private:
  ChannelBuffer<M0> buffer_;
};

}
}
}

#endif