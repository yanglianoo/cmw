#ifndef CMW_BASE_FOR_EACH_H_
#define CMW_BASE_FOR_EACH_H_
#include <type_traits>
#include <iostream>
#include <cmw/base/macros.h>
namespace hnu    {
namespace cmw   {
namespace base {

DEFINE_TYPE_TRAIT(HasLess, operator<)  // NOLINT

template <class Value, class End>
typename std::enable_if<HasLess<Value>::value && HasLess<End>::value,
                        bool>::type
LessThan(const Value& val, const End& end) {
  return val < end;
}

template <class Value, class End>
typename std::enable_if<!HasLess<Value>::value || !HasLess<End>::value,
                        bool>::type
LessThan(const Value& val, const End& end) {
  return val != end;
}


#define FOR_EACH(i, begin, end)           \
  for (auto i = (true ? (begin) : (end)); \
       hnu::cmw::base::LessThan(i, (end)); ++i)

}
}
}








#endif