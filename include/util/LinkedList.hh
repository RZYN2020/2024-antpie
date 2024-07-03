#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include <memory>

template <typename T>
struct Node {
  T data;
  Node* next;

  Node(const T& data) : data(data), next(nullptr) {}
};

template <typename T>
class LinkedList {
 private:
  Node<T>* head;
  Node<T>* tail;
  int size;

 public:
  LinkedList() : head(nullptr), tail(nullptr), size(0) {}
  ~LinkedList() {
    Node<T>* current = head;
    Node<T>* next;
    while (current != nullptr) {
      next = current->next;
      delete current;
      current = next;
    }
  }

  Node<T>* getHead() const { return head; }
  Node<T>* getTail() const { return tail; }

  T front() { return head ? head->data : nullptr; }
  T back() { return tail ? tail->data : nullptr; }

  void pushFront(const T& data) {
    Node<T>* newNode = new Node<T>(data);
    newNode->next = head;
    head = newNode;
    if (tail == nullptr) {
      tail = newNode;
    }
    size++;
  }
  void pushBack(const T& data) {
    Node<T>* newNode = new Node<T>(data);
    if (head == nullptr) {
      head = newNode;
      tail = newNode;
    } else {
      tail->next = newNode;
      tail = newNode;
    }
    size++;
  }
  void remove(const T& data) {
    Node<T>* current = head;
    Node<T>* previous = nullptr;

    while (current != nullptr && current->data != data) {
      previous = current;
      current = current->next;
    }

    if (current == nullptr) {
      return;
    }

    if (previous == nullptr) {
      head = current->next;
    } else {
      previous->next = current->next;
    }

    if (current == tail) {
      tail = previous;
    }

    delete current;
    size--;
  }

  // it is better not use this function
  // It's O(n), but popFront() is O(1)
  T popBack() {
    T old = tail->data;
    remove(old);
    return old;
  }

  T popFront() {
    T old = head->data;
    remove(old);
    return old;
  } 

  bool isEmpty() const { return head == nullptr; }

  int getSize() const { return size; }

  // 定义迭代器类
  class Iterator : public std::iterator<std::forward_iterator_tag, T> {
   private:
    Node<T>* current;

   public:
    Iterator(Node<T>* node) : current(node) {}

    Iterator& operator++() {
      if (current) {
        current = current->next;
      }
      return *this;
    }

    Iterator operator+(int n) const {
      Iterator temp = *this;
      for (int i = 0; i < n && temp.current; ++i) {
        temp.current = temp.current->next;
      }
      return temp;
    }

    T& operator*() const { return current->data; }

    bool operator!=(const Iterator& other) const {
      return current != other.current;
    }
  };

  Iterator begin() const { return Iterator(head); }

  Iterator end() const { return Iterator(nullptr); }
};

#endif