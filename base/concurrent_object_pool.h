#ifndef CMW_BASE_CONCURRENT_OBJECT_POOL_H_
#define CMW_BASE_CONCURRENT_OBJECT_POOL_H_


#include <atomic>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <cmw/base/for_each.h>
#include <cmw/base/macros.h>

namespace hnu    {
namespace cmw   {
namespace base {


template <typename T>
class CCObjectPool : public std::enable_shared_from_this<CCObjectPool<T>> {
 public:
    explicit CCObjectPool(uint32_t size);
    virtual ~CCObjectPool();

    template <typename... Args>
    void ConstructAll(Args&&... args);

    template <typename... Args>
    std::shared_ptr<T> ConstructObject(Args&&... args);

    std::shared_ptr<T> GetObject();
    void ReleaseObject(T *);
    uint32_t size() const;

 private:
    struct Node
    {
        T object;
        Node *next;
    };

    struct alignas(2 * sizeof(Node*)) Head{
        uintptr_t count;
        Node *node;
    };

 private:
    CCObjectPool(CCObjectPool &) = delete;
    CCObjectPool &operator=(CCObjectPool &) = delete;
    bool FindFreeHead(Head *head);

    std::atomic<Head> free_head_;
    Node *node_arena_ = nullptr;

    uint32_t capacity_ = 0;
};

template <typename T>
CCObjectPool<T>::CCObjectPool(uint32_t size) : capacity_(size) {
    node_arena_ = static_cast<Node *>(CheckedCalloc(capacity_, sizeof(Node)));
    FOR_EACH(i, 0, capacity_ - 1){
        node_arena_[i].next = node_arena_ + 1 + i;
    }

    node_arena_[capacity_ - 1].next = nullptr;
    free_head_.store({0 , node_arena_}, std::memory_order_relaxed);
}

template <typename T>
template <typename... Args>
void CCObjectPool<T>::ConstructAll(Args &&... args){
    FOR_EACH(i, 0, capacity_){
        new (node_arena_ + i) T(std::forward<Args>(args)...);
    }
}

template <typename T>
CCObjectPool<T>::~CCObjectPool() {
  std::free(node_arena_);
}

template <typename T>
bool CCObjectPool<T>::FindFreeHead(Head *head){
    Head new_head;
    Head old_head = free_head_.load(std::memory_order_acquire);

    do{
        if(cyber_likely(old_head.node == nullptr)){
            return false;
        }
        new_head.node = old_head.node->next;
        new_head.count = old_head.count + 1;
    } while (!free_head_.compare_exchange_weak(old_head, new_head,
                                               std::memory_order_acq_rel,
                                               std::memory_order_acquire));
    {
        *head = old_head;
        return true;
    }  
}

template <typename T>
std::shared_ptr<T> CCObjectPool<T>::GetObject() {
    Head free_head;
    
    // 尝试从对象池中找到一个空闲的节点
    if (cyber_unlikely(!FindFreeHead(&free_head))) {
        return nullptr;  // 如果找不到空闲节点，返回 nullptr 表示对象池已经空了
    }

    // 获取当前对象池的 shared_ptr 指针
    auto self = this->shared_from_this();

    // 创建并返回一个 shared_ptr，指向空闲节点中的对象 , lambda 表达式为自定义的删除器
    return std::shared_ptr<T>(reinterpret_cast<T *>(free_head.node),
                              [self](T *object) { self->ReleaseObject(object); });
}

/**
 * @brief  ReleaseObject 函数的目标是将一个使用完毕的对象 object 放回对象池。
 *         对象池中使用了链表来管理空闲对象节点，每次释放对象时，需要将其重新加入空闲链表的头部。
 * @tparam T 
 * @param  object: 
 */
template <typename T>
void CCObjectPool<T>::ReleaseObject(T *object) {
  Head new_head;
  Node *node = reinterpret_cast<Node *>(object);
  Head old_head = free_head_.load(std::memory_order_acquire);
  do {
    node->next = old_head.node;
    new_head.node = node;
    new_head.count = old_head.count + 1;
  } while (!free_head_.compare_exchange_weak(old_head, new_head,
                                             std::memory_order_acq_rel,
                                             std::memory_order_acquire));
}

}
}
}


#endif