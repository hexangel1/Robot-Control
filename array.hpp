#ifndef ARRAY_HPP_SENTRY
#define ARRAY_HPP_SENTRY

#include <cstddef>

template <class T>
class Array {
        T **ptr;
        size_t used;
        size_t size;
        bool is_owner;
public:
        Array(bool owner = true) : ptr(0), used(0), size(0), is_owner(owner) {}
        ~Array();
        void Add(T *elem);
        size_t Size() const { return used; }
        T *operator[](size_t idx) const { return ptr[idx]; }
};

template <class T>
Array<T>::~Array()
{
        size_t i;
        if (is_owner) {
                for (i = 0; i < used; i++)
                        delete ptr[i];
        }
        delete[] ptr;
}

template <class T>
void Array<T>::Add(T *elem) 
{
        if (used == size) {
                size_t old_size = size;
                size = size > 0 ? size << 1 : 8;
                T **tmp = new T*[size];
                for (size_t i = 0; i < old_size; i++)
                        tmp[i] = ptr[i];
                if (ptr)
                        delete[] ptr;
                ptr = tmp;
        }
        ptr[used] = elem;
        used++;
}

#endif

