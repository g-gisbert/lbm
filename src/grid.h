#ifndef EFS_GRID_H
#define EFS_GRID_H

#include <vector>

template <typename T>
class Grid {
public:
    Grid(int w, int h, T initValue) : m_width(w), m_height(h), m_initValue(initValue), m_data(w*h, initValue) {}

    void reset() {
        for (int i = 0;  i < m_width * m_height; ++i) {
            m_data[i] = m_initValue;
        }
    }

    T operator[](int i) const {
        return m_data[i];
    }

    T& operator[](int i) {
        return m_data[i];
    }

    T operator()(int i, int j) const {
        return m_data[i * m_width + j];
    }

    T& operator()(int i, int j) {
        return m_data[i * m_width + j];
    }

    const T* raw() const {
        return m_data.data();
    }
    T* raw() {
        return m_data.data();
    }

    typename std::vector<T>::iterator begin() {
        return std::begin(m_data);
    }

    typename std::vector<T>::iterator end() {
        return std::end(m_data);
    }


private:
    int m_width;
    int m_height;
    T m_initValue;
    std::vector<T> m_data;
};


#endif //EFS_GRID_H
