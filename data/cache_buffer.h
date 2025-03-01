#ifndef CMW_DATA_CACHE_BUFFER_H_
#define CMW_DATA_CACHE_BUFFER_H_

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace hnu {
namespace cmw {
namespace data{

template <typename T>
class CacheBuffer {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using FusionCallback = std::function<void(const T&)>;

        explicit CacheBuffer(uint64_t size){
            capacity_ = size + 1;
            buffer_.resize(capacity_);
        }

        CacheBuffer(const CacheBuffer& rhs) {
            std::lock_guard<std::mutex> lg(rhs.mutex_);
            head_ = rhs.head_;
            tail_ = rhs.tail_;
            buffer_ = rhs.buffer_;
            capacity_ = rhs.capacity_;
            fusion_callback_ = rhs.fusion_callback_;
        }

        T& operator[](const uint64_t& pos) { return buffer_[GetIndex(pos)]; }
        const T& at(const uint64_t& pos) const {return buffer_[GetIndex(pos)];}
        
        uint64_t Head() const { return head_ + 1; }
        uint64_t Tail() const { return tail_; }
        uint64_t Size() const { return tail_ - head_; }

        const T& Front() const { return buffer_[GetIndex(head_ + 1)]; }
        const T& Back() const { return buffer_[GetIndex(tail_)]; }

        bool Empty() const { return tail_ == 0; }
        bool Full() const { return capacity_ - 1 == tail_ - head_; }
        uint64_t Capacity() const { return capacity_; }

        void SetFusionCallback(const FusionCallback& callback) {
            fusion_callback_ = callback;
        }

        void Fill(const T& value){
            if(fusion_callback_){
                fusion_callback_(value);
            }else{
                if(Full()){
                    buffer_[GetIndex(head_)] = value;
                    ++head_;
                    ++tail_;
                }else{
                    buffer_[GetIndex(tail_ +1)] = value;
                    ++tail_;
                }
            }
        }

        std::mutex& Mutex() { return mutex_; }
    private:
        CacheBuffer& operator=(const CacheBuffer& other) = delete;
        uint64_t GetIndex(const uint64_t& pos) const { return pos % capacity_; }
        
        uint64_t head_ = 0;
        uint64_t tail_ = 0;
        uint64_t capacity_ = 0;
        std::vector<T> buffer_;
        mutable std::mutex mutex_;
        FusionCallback fusion_callback_;
};

}
}
}


#endif