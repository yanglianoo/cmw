#ifndef CMW_TRANSPORT_SHM_READABLE_INFO_H_
#define CMW_TRANSPORT_SHM_READABLE_INFO_H_


#include <stdint.h>
#include <string>
namespace hnu{
namespace cmw{
namespace transport{

class ReadableInfo{

public:
    ReadableInfo();
    ReadableInfo(uint64_t host_id , uint32_t block_index , uint64_t channel_id);
    virtual ~ReadableInfo();

    ReadableInfo& operator=(const ReadableInfo& other);

    bool DeserializeFrom(const std::string& src);
    bool DeserializeFrom(const char* src , std::size_t len);
    bool SerializeTo(std::string* dst) const;

    uint64_t host_id() const { return host_id_; }
    void ser_host_id(uint64_t host_id) { host_id_ = host_id; }

    uint32_t block_index() const { return block_index_; }
    void set_block_index(uint32_t block_index) { block_index_ = block_index; }

    uint64_t channel_id() const { return channel_id_; }
    void set_channel_id(uint64_t channel_id) { channel_id_ = channel_id; }

    static const size_t ksize;
private:
    uint64_t host_id_;
    uint32_t block_index_;
    uint64_t channel_id_;
    
};




}
}
}


#endif