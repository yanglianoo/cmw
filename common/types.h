#ifndef CMW_COMMON_TYPES_H_
#define CMW_COMMON_TYPES_H_


#include <cstdint>
namespace hnu {
namespace cmw {

class NullType {};

/**
 * @brief Describe relation between nodes, writers/readers...
 */
enum Relation : std::uint8_t {
  NO_RELATION = 0,
  DIFF_HOST,  // different host
  DIFF_PROC,  // same host, but different process
  SAME_PROC,  // same process
};

}
}

#endif