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

  void clear() {
    Node<T>* current = head;
    Node<T>* next = nullptr;

    while (current != nullptr) {
      next = current->next;
      delete current;
      current = next;
    }

    head = nullptr;
    tail = nullptr;
    size = 0;
  }

  // defin iterator class
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

    bool operator==(const Iterator& other) const {
      return current == other.current;
    }

    Node<T>* getCurrentNode() const { return current; }
  };

  Iterator begin() const { return Iterator(head); }

  Iterator end() const { return Iterator(nullptr); }

  void insertBefore(const Iterator& it, const T& data) {
    if (it == begin()) {
      pushFront(data);
      return;
    }

    Node<T>* newNode = new Node<T>(data);
    Node<T>* current = head;
    Node<T>* previous = nullptr;

    while (current != nullptr && current != it.getCurrentNode()) {
      previous = current;
      current = current->next;
    }

    if (current == nullptr) {
      delete newNode;
      return;
    }

    previous->next = newNode;
    newNode->next = current;
    size++;
  }

  void insertBefore(const T& loc, const T& data) {
    if (head && loc == head->data) {
      pushFront(data);
      return;
    }

    Node<T>* newNode = new Node<T>(data);
    Node<T>* current = head;
    Node<T>* previous = nullptr;

    while (current != nullptr && current->data != loc) {
      previous = current;
      current = current->next;
    }

    if (current == nullptr) {
      delete newNode;
      return;
    }

    previous->next = newNode;
    newNode->next = current;
    size++;
  }

  void insertAfter(const Iterator& it, const T& data) {
    if (it == end()) {
      return;
    }

    Node<T>* newNode = new Node<T>(data);
    Node<T>* current = it.getCurrentNode();

    if (current == tail) {
      tail = newNode;
    }

    newNode->next = current->next;
    current->next = newNode;
    size++;
  }

  /**
   * @brief Splits the linked list into two parts after the given iterator
   * position.
   *
   * This function takes an iterator and splits the linked list into two
   * separate lists. The original list retains the elements from the beginning
   * up to the given iterator position. The new list (provided by the caller) is
   * assigned the elements that come after the iterator position.
   *
   */
  void splitAfter(const Iterator& it, LinkedList<T>* newList) {
    if (newList == nullptr || it == end() || it.getCurrentNode() == tail) {
      return;
    }

    Node<T>* current = it.getCurrentNode();
    Node<T>* newHead = current->next;

    current->next = nullptr;
    newList->head = newHead;
    newList->tail = tail;

    // Calculate the size of the new list and update the original list size
    int newSize = 0;
    while (newHead != nullptr) {
      newSize++;
      newHead = newHead->next;
    }

    newList->size = newSize;
    this->size -= newSize;
    this->tail = current;
  }
};

#endif