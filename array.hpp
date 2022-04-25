#ifndef ARRAY_HPP_SENTRY
#define ARRAY_HPP_SENTRY

#include <cstddef>

template <class T>
class Array {
        T **ptr;
        size_t used;
        size_t size;
public:
        Array() : ptr(0), used(0), size(0) {}
        ~Array();
        void Add(T *elem);
        size_t Size() const { return used; }
        T *operator[](size_t idx) const { return ptr[idx]; }
};

template <class T>
Array<T>::~Array()
{
        size_t i;
        for (i = 0; i < used; i++)
                delete ptr[i];
        delete[] ptr;
}

template <class T>
void Array<T>::Add(T *elem) 
{
        if (used == size) {
                size_t old_size = size;
                size += 4;
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

