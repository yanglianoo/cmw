#ifndef CMW_DATA_FUSION_DATA_FUSION_H_
#define CMW_DATA_FUSION_DATA_FUSION_H_

#include <deque>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include <cmw/common/types.h>

namespace hnu {
namespace cmw {
namespace data{
namespace fusion{


template <typename M0, typename M1 = NullType, typename M2 = NullType,
          typename M3 = NullType>
class DataFusion{
    public:
        virtual ~DataFusion() {}
        virtual bool Fusion(uint64_t* index, std::shared_ptr<M0>& m0,
                            std::shared_ptr<M1>& m1,
                            std::shared_ptr<M2>& m2,
                            std::shared_ptr<M3>& m3) = 0;
};

// 特化版本，三个模板参数
template <typename M0, typename M1, typename M2>
class DataFusion<M0, M1, M2, NullType> {
 public:
  virtual ~DataFusion() {}

  virtual bool Fusion(uint64_t* index, std::shared_ptr<M0>& m0,  // NOLINT
                      std::shared_ptr<M1>& m1,                   // NOLINT
                      std::shared_ptr<M2>& m2) = 0;              // NOLINT
};

// 特化版本，两个模板参数
template <typename M0, typename M1>
class DataFusion<M0, M1, NullType, NullType> {
 public:
  virtual ~DataFusion() {}

  virtual bool Fusion(uint64_t* index, std::shared_ptr<M0>& m0,  // NOLINT
                      std::shared_ptr<M1>& m1) = 0;              // NOLINT
};

}
}
}
}

#endif