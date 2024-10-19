#pragma once

template<typename T, typename Compare>
void heap<T, Compare>::push(T &element) {
    m_heap.push_back(element);
    sort_down(0, m_heap.size() - 1);
}

template<typename T, typename Compare>
T heap<T, Compare>::pop() {
    auto last = m_heap.back();
    m_heap.pop_back();
    if (!empty()) {
        auto ret = top();
        m_heap[0] = last;
        sort_up(0);
        return ret;
    }
    return last;
}

template<typename T, typename Compare>
T heap<T, Compare>::replace(T element) {
    auto ret = top();
    m_heap[0] = element;
    sort_up(0);
    return ret;
}

template<typename T, typename Compare>
T heap<T, Compare>::push_pop(T &element) {
    if (!empty() && cmp(top(), element)) {
        std::swap(element, m_heap[0]);
        sort_up(0);
    }
    return element;
}

template<typename T, typename Compare>
void heap<T, Compare>::heapify() {
    for (int i = static_cast<int>(m_heap.size() / 2) - 1; i >= 0; --i) {
        sort_up(i);
    }
}

template<typename T, typename Compare>
T heap<T, Compare>::top() {
    return m_heap.front();
}

template<typename T, typename Compare>
int heap<T, Compare>::size() {
    return m_heap.size();
}

template<typename T, typename Compare>
bool heap<T, Compare>::empty() {
    return m_heap.empty();
}

template<typename T, typename Compare>
void heap<T, Compare>::sort_down(const int start, int pos) {
    auto item = m_heap.at(pos);

    while (pos > start) {
        const auto parent_pos = (pos - 1) >> 1;
        auto parent = m_heap.at(parent_pos);
        if (cmp(item, parent)) {
            m_heap[pos] = parent;
            pos = parent_pos;
            continue;
        }
        break;
    }
    m_heap[pos] = item;
}

template<typename T, typename Compare>
void heap<T, Compare>::sort_up(int pos) {
    const auto end = m_heap.size();
    const auto start = pos;
    auto item = m_heap.at(pos);
    int child_pos = 2 * pos + 1;
    while (child_pos < end) {
        if (int right_pos = child_pos + 1; right_pos < end && !cmp(m_heap.at(child_pos), m_heap.at(right_pos))) {
            child_pos = right_pos;
        }
        m_heap[pos] = m_heap[child_pos];
        pos = child_pos;
        child_pos = 2 * pos + 1;
    }

    m_heap[pos] = item;
    sort_down(start, pos);
}
