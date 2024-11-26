#ifndef CMW_INIT_H_
#define CMW_INIT_H_

#include <cmw/common/log.h>
#include <cmw/state.h>

namespace hnu {
namespace cmw {

bool Init(const char* binary_name);
void Clear();

}
}
#endif