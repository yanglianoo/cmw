#ifndef CMW_DISCOVERY_CONTAINER_MULTI_VALUE_WAREHOUSE_H_
#define CMW_DISCOVERY_CONTAINER_MULTI_VALUE_WAREHOUSE_H_

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <cmw/base/atomic_rw_lock.h>
#include <cmw/discovery/container/warehouse_base.h>
namespace hnu {
namespace cmw {
namespace discovery{ 

using namespace base;
class MultiValueWarehouse : public WarehouseBase {
 public:
  using RoleMap = std::unordered_multimap<uint64_t, RolePtr>;

  MultiValueWarehouse() {}
  virtual ~MultiValueWarehouse() {}

  bool Add(uint64_t key, const RolePtr& role,
           bool ignore_if_exist = true) override;

  void Clear() override;
  std::size_t Size() override;

  void Remove(uint64_t key) override;
  void Remove(uint64_t key, const RolePtr& role) override;
  void Remove(const RoleAttributes& target_attr) override;

  bool Search(uint64_t key) override;
  bool Search(uint64_t key, RolePtr* first_matched_role) override;
  bool Search(uint64_t key,
              RoleAttributes* first_matched_role_attr) override;
  bool Search(uint64_t key, std::vector<RolePtr>* matched_roles) override;
  bool Search(uint64_t key,
              std::vector<RoleAttributes>* matched_roles_attr) override;

  bool Search(const RoleAttributes& target_attr) override;
  bool Search(const RoleAttributes& target_attr,
              RolePtr* first_matched) override;
  bool Search(const RoleAttributes& target_attr,
              RoleAttributes* first_matched_role_attr) override;
  bool Search(const RoleAttributes& target_attr,
              std::vector<RolePtr>* matched_roles) override;
  bool Search(const RoleAttributes& target_attr,
              std::vector<RoleAttributes>* matched_roles_attr) override;

  void GetAllRoles(std::vector<RolePtr>* roles) override;
  void GetAllRoles(std::vector<RoleAttributes>* roles_attr) override;

 private:
  RoleMap roles_;
  AtomicRWLock rw_lock_;
};

}
}
}

#endif