/**
 * @File Name: identity.h
 * @brief  
 * @Author : Timer email:330070781@qq.com
 * @Version : 1.0
 * @Creat Date : 2023-12-05
 * 
 */
#ifndef HNU_TRANSPORT_COMMON_IDENTITY_H_
#define HNU_TRANSPORT_COMMON_IDENTITY_H_

#include <cstdint>
#include <cstring>
#include <string>

namespace hnu    {
namespace cmw   {
namespace transport {

    
constexpr uint8_t ID_SIZE = 8;
/* 标识符类 */
class Identity
{
public:
    explicit Identity(bool need_generate = true);
    Identity(const Identity& another);
    virtual ~Identity();


    Identity& operator=(const Identity& another);
    bool operator==(const Identity& another) const;
    bool operator!=(const Identity& another) const;

    /*拿到标识符的属性值*/
    std::string ToString() const;
    size_t Length() const;
    uint64_t HashValue() const;
    const char* data() const { return data_; }

    /*设置标识符的值*/
    void set_data(const char* data) {
        if (data == nullptr) {
        return;
        }
        std::memcpy(data_, data, sizeof(data_));
        Update();
    }

private:
    void Update();          // 更新标识符
    char data_[ID_SIZE];    // 8个字节
    uint64_t hash_value_;   // 标识符的哈希值

};

}
}
}

#endif