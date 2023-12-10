#ifndef CMW_BASE_UNBOUNDED_QUEUE_H_
#define CMW_BASE_UNBOUNDED_QUEUE_H_

#include <unistd.h>

#include <atomic>
#include <cstdint>
#include <memory>

namespace hnu    {
namespace cmw   {
namespace base {

template <typename T>
class UnboundedQueue {
 public:
  UnboundedQueue() { Reset(); }
  UnboundedQueue& operator=(const UnboundedQueue& other) = delete;
  UnboundedQueue(const UnboundedQueue& other) = delete;

  ~UnboundedQueue() { Destroy(); }

  void Clear() {
    Destroy();
    Reset();
  }

  void Enqueue(const T& element) {
    auto node = new Node();
    node->data = element;
    Node* old_tail = tail_.load();

    while (true) {
      if (tail_.compare_exchange_strong(old_tail, node)) {
        old_tail->next = node;
        old_tail->release();
        size_.fetch_add(1);
        break;
      }
    }
  }

  bool Dequeue(T* element) {
    Node* old_head = head_.load();
    Node* head_next = nullptr;
    do {
      head_next = old_head->next;

      if (head_next == nullptr) {
        return false;
      }
    } while (!head_.compare_exchange_strong(old_head, head_next));
    *element = head_next->data;
    size_.fetch_sub(1);
    old_head->release();
    return true;
  }

  size_t Size() { return size_.load(); }

  bool Empty() { return size_.load() == 0; }

 private:
  struct Node {
    T data;
    std::atomic<uint32_t> ref_count;
    Node* next = nullptr;
    Node() { ref_count.store(2); }
    void release() {
      ref_count.fetch_sub(1);
      if (ref_count.load() == 0) {
        delete this;
      }
    }
  };

  void Reset() {
    auto node = new Node();
    head_.store(node);
    tail_.store(node);
    size_.store(0);
  }

  void Destroy() {
    auto ite = head_.load();
    Node* tmp = nullptr;
    while (ite != nullptr) {
      tmp = ite->next;
      delete ite;
      ite = tmp;
    }
  }

  std::atomic<Node*> head_;
  std::atomic<Node*> tail_;
  std::atomic<size_t> size_;
};
}
}
}



#endif