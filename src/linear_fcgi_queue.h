// linear::fcgi::Queue works with single consumer-producer model
// ref: http://www.soa-world.de/echelon/2011/02/a-simple-lock-free-queue-in-c.html

#ifndef _LINEAR_FCGI_QUEUE_H_
#define _LINEAR_FCGI_QUEUE_H_

namespace linear {
namespace fcgi {

template <typename T>
class Queue {
public:
  Queue() {
    first = new Node(T());
    divider = first;
    last = first;
  }

  ~Queue() {
    while (first != NULL) {
      Node* tmp = first;
      first = tmp->next;
      delete tmp;
    }
  }

  void produce(const T& t) {
    last->next = new Node(t);
    asm volatile("" ::: "memory");
    (void)__sync_lock_test_and_set(&last, last->next);
    while (first != divider) {
      Node* tmp = first;
      first = first->next;
      delete tmp;
    }
    return;
  }

  bool consume(T& result) {
    if (divider != last) {
      result = divider->next->value;
      asm volatile("" ::: "memory");
      (void)__sync_lock_test_and_set(&divider, divider->next);
      return true;
    }
    return false;
  }

  bool empty() {
    return (divider == last);
  }

  size_t size() {
    size_t num = 0;
    const Node* it = divider->next;
    while (it) {
      it = it->next;
      num++;
    }
    return num;
  }

private:
  struct Node {
    Node(T val) : value(val), next(NULL) {}
    ~Node() {}
    T value;
    Node* next;
  };
  Node* first;
  Node* divider;
  Node* last;
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_QUEUE_HPP_
