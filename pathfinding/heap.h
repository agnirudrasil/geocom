#ifndef HEAP_H
#define HEAP_H
#include <vector>


template<typename T, typename Compare = std::less<T> >
class heap {
public:
    heap() = default;

    heap(std::initializer_list<T> elements) {
        for (auto el: elements) m_heap.push_back(el);
        heapify();
    }

    void push(T &element);

    T pop();

    T replace(T element);

    T push_pop(T &element);

    void heapify();

    T top();

    int size();

    bool empty();

private:
    std::vector<T> m_heap;
    constexpr static Compare cmp{};

    void sort_down(int start, int pos);

    void sort_up(int pos);
};

#include "heap.tpp"
#endif //HEAP_H
