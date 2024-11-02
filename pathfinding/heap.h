#ifndef HEAP_H
#define HEAP_H
#include <vector>

/**
 * @class heap
 * @brief A templated heap data structure that provides priority queue functionalities.
 *
 * @tparam T The type of elements stored in the heap.
 * @tparam Compare A comparison functor used to order the heap, defaults to std::less<T>.
 */
template<typename T, typename Compare = std::less<T> >
class heap {
    std::vector<T> m_heap; ///< The underlying container for storing the heap elements.
    constexpr static Compare cmp{}; ///< The comparison object for ordering the heap.

public:
    /**
     * @brief Default constructor for an empty heap.
     */
    heap() = default;

    /**
     * @brief Constructs a heap with a given list of elements.
     *
     * @param elements List of elements that the heap should contain initially.
     */
    heap(std::initializer_list<T> elements);

    /**
     * @brief Adds an element to the heap.
     *
     * @param element The element to add to the heap (lvalue reference).
     */
    void push(T &element);

    /**
     * @brief Adds an element to the heap.
     *
     * @param element The element to add to the heap (rvalue reference).
     */
    void push(T &&element);

    /**
     * @brief Removes and returns the top element of the heap.
     *
     * @return The top element of the heap.
     */
    T pop();

    /**
     * @brief Replaces the top element with a new element and returns the old top.
     *
     * @param element The element to replace the top with.
     * @return The old top element of the heap.
     */
    T replace(T element);

    /**
     * @brief Pushes an element onto the heap and then pops and returns the top element.
     *
     * @param element The element to push onto the heap (lvalue reference).
     * @return The top element after the push operation.
     */
    T push_pop(T &element);

    /**
     * @brief Arranges the elements to satisfy the heap property.
     */
    void heapify();

    /**
     * @brief Returns the top element of the heap without removing it.
     *
     * @return The top element of the heap.
     */
    T top();

    /**
     * @brief Returns the number of elements in the heap.
     *
     * @return The size of the heap.
     */
    int size();

    /**
     * @brief Checks whether the heap is empty.
     *
     * @return True if the heap is empty, false otherwise.
     */
    bool empty();

private:
    /**
     * @brief Helper function to maintain the heap property by sorting downwards.
     *
     * @param start The start index for the down-sorting.
     * @param pos The position to maintain the heap property from.
     */
    void sort_down(int start, int pos);

    /**
     * @brief Helper function to maintain the heap property by sorting upwards.
     *
     * @param pos The position to maintain the heap property from.
     */
    void sort_up(int pos);
};

#include "heap.tpp"
#endif //HEAP_H
