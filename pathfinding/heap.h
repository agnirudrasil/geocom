#ifndef HEAP_H
#define HEAP_H
#include <vector>


template<typename T>
class heap {
    std::vector<T> m_heap;

    void sort_down(const int start, int pos) {
        auto item = m_heap.at(pos);

        while (pos > start) {
            const auto parent_pos = (pos - 1) >> 1;
            auto parent = m_heap.at(parent_pos);
            if (item < parent) {
                m_heap[pos] = parent;
                pos = parent_pos;
                continue;
            }
            break;
        }
        m_heap[pos] = item;
    };

    void sort_up(int pos) {
        const auto end = size();
        const auto start = pos;

        auto item = m_heap.at(pos);

        int child_pos = 2 * pos + 1;
        while (child_pos < end) {
            if (int right_pos = child_pos + 1; right_pos < end && !(m_heap.at(child_pos) < m_heap.at(right_pos))) {
                child_pos = right_pos;
            }
            m_heap[pos] = m_heap[child_pos];
            pos = child_pos;
            child_pos = 2 * pos + 1;
        }

        m_heap[pos] = item;
        sort_down(start, pos);
    };

public:
    heap() = default;


    heap(std::initializer_list<T> elements) {
        for (auto el: elements) m_heap.push_back(el);
        heapify();
    };

    void push(T element) {
        m_heap.push_back(element);
        sort_down(0, m_heap.size() - 1);
    }

    T pop() {
        auto last = m_heap.back();
        m_heap.pop_back();
        if (!empty()) {
            auto ret = top();
            m_heap[0] = last;
            sort_up(0);
            return ret;
        }
        return last;
    };

    T replace(T element) {
        auto ret = top();
        m_heap[0] = element;
        sort_up(0);
        return ret;
    };

    T push_pop(T element) {
        if (!empty() && top() < element) {
            std::swap(element, m_heap[0]);
            sort_up(0);
        }
        return element;
    };

    void heapify() {
        for (int i = (m_heap.size() / 2) - 1; i >= 0; --i) {
            sort_up(i);
        }
    };

    T top() {
        return m_heap.front();
    };

    int size() {
        return m_heap.size();
    };

    bool empty() {
        return m_heap.empty();
    };
};


#endif //HEAP_H
