// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

/**
 * @file max_deque.hpp
 * @brief Implementation of the max_deque container allowing constant access to its maximum element.
 */

#ifndef CPP_MAX_DEQUE_H_
#define CPP_MAX_DEQUE_H_

#include <deque>
#include <ostream>


//! @brief Deque allowing constant access to maximum element.
template <typename T, typename C = std::less<T>>
class max_deque {
  public:
    //! @brief construction
    //! @{
    max_deque() = default;
    
    template <class I>
    max_deque(I first, I last) {
        for (I it = first; it != last; ++it)
            push_back(*it);
    }
    
    max_deque(std::initializer_list<T> il) {
        for (const T& x : il)
            push_back(x);
    }
    //! @}

    //! @brief copy and assignment
    //! @{
    max_deque(const max_deque&) = default;
    
    max_deque(max_deque&&) = default;
    
    max_deque& operator=(const max_deque&) = default;
    
    max_deque& operator=(max_deque&&) = default;
    //! @}

    //! @brief tests whether the container is empty
    bool empty() const {
        return m_end > m_begin;
    }
    
    //! @brief elements virtually in the container
    size_t size() const {
        return m_end - m_begin;
    }
    
    //! @brief clears the queue
    void clear() {
        m_data.clear();
        m_begin = m_end = 0;
    }
    
    //! @brief the first index in the queue
    size_t front() const {
        return m_begin;
    }
    
    //! @brief the last index in the queue
    size_t back() const {
        return m_end-1;
    }
    
    //! @brief accesses the maximal element in the queue
    const T& top() const {
        return m_data.front().first;
    }
    
    //! @brief removes the first element in the queue
    void pop_front() {
        if (m_data.front().second == m_begin) m_data.pop_front();
        ++m_begin;
    }

    //! @brief inserts elements at the beginning of the queue
    //! @{
    void push_front(const T& x) {
        --m_begin;
        if (m_compare(m_data.front().first, x))
            m_data.emplace_front(x, m_begin);
    }
    void push_front(T&& x) {
        --m_begin;
        if (m_compare(m_data.front().first, x))
            m_data.emplace_front(x, m_begin);
    }
    template <class... Ts>
    void emplace_front(Ts&&... xs) {
        push_front(T(xs...));
    }
    //! @}

    //! @brief inserts elements at the end of the queue
    //! @{
    void push_back(const T& x) {
        while (not m_data.empty() and not m_compare(x, m_data.back().first))
            m_data.pop_back();
        m_data.emplace_back(x, m_end);
        ++m_end;
    }
    void push_back(T&& x) {
        while (not m_data.empty() and not m_compare(x, m_data.back().first))
            m_data.pop_back();
        m_data.emplace_back(x, m_end);
        ++m_end;
    }
    template <class... Ts>
    void emplace_back(Ts&&... xs) {
        push_back(T(xs...));
    }
    //! @}

  private:
    //! @brief candidate maxima with indices
    std::deque<std::pair<T,size_t>> m_data;
    //! @brief virtual index of beginning
    size_t m_begin = 0;
    //! @brief virtual index of end
    size_t m_end = 0;
    //! @brief comparator
    C m_compare;
};


//! @brief printing
template <typename T, typename C>
std::ostream& operator<<(std::ostream& o, const max_deque<T,C>& q) {
    return o << "[" << q.front() << ".." << q.back() << ": T = " << q.top() << "]";
}


#endif // CPP_MAX_DEQUE_H_
