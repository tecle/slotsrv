#ifndef __CIRCULAR_QUEUE_H_
#define __CIRCULAR_QUEUE_H_

#include <assert.h>
#include <functional>

namespace cgserver 
{

template<typename T>
class CircularQueue
{
public:
    CircularQueue(size_t capacity = 4 * 1024)
    {
        //assert(capacity < ((1 << 31) - 1));
        _items = new T[capacity];
        _capacity = capacity;
        _size = 0;
        _front = 0; 
        _back = 0;
    }

    ~CircularQueue() {delete []_items;}
private:
    CircularQueue& operator = (const CircularQueue &);
public:
    inline void push_front(const T &t) {
        if (_size == 0) {
            _front = 0;
            _back = 0;
        } else if (--_front == -1) {
            _front = _capacity - 1;
        }
        ++_size;
        _items[_front] = t;
    }

    inline void push_back(const T &t) {
        if (_size == 0) {
            _front = 0;
            _back = 0;
        } else if (++_back == _capacity) {
            _back = 0;
        }
        ++_size;
        _items[_back] = t;
    }

    inline void pop_front() {
        assert(_size);
        if (++_front == _capacity) {
            _front = 0;
        }
        --_size;
    }

    inline void pop_back() {
        assert(_size);
        if (--_back == -1) {
            _back = _capacity - 1;
        }
        --_size;
    }

    inline void reset() {
        _size = 0;
        _front = 0;
        _back = 0;
    }

    inline void set_capacity(size_t capacity)
    {
        //assert(capacity < ((1 << 31) - 1));
        if (_items) delete []_items;
        _items = new T[capacity];
        _capacity = capacity;
        _size = 0;
        _front = 0;
        _back = 0;
    }

    T* find(std::function<bool(T *cur)> cmp) {
            size_t i = _front;
      while (true){
          T *tmp = &(_items[i]);
          if (cmp(tmp)) {
              return tmp;
          }
          if (i == _back) break;
          if (++i == _capacity) i = 0;
      }
      return nullptr;
    }

    inline T& front() {return _items[_front];} 
    inline T& back() {return _items[_back];}
    inline T& at(size_t idx) {return _items[idx];}

    inline const T& front() const {return _items[_front];} 
    inline const T& back() const {return _items[_back];} 

    inline size_t size() const {return _size;}
    inline size_t capacity() const {return _capacity;}
    inline bool empty() const {return _size == 0;}
private:
    T *_items;
    int32_t _capacity;
    int32_t _size;
    int32_t _front;
    int32_t _back;
};

}
#endif //
