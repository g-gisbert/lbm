#ifndef EFS_STAGGEREDGRID_H
#define EFS_STAGGEREDGRID_H

#include <vector>

template<class T>
class StaggeredGrid {
public:
    StaggeredGrid(int w, int h, T initValue) : m_width(w), m_height(h),
                                               m_dataHorizontal(w*(h+1), initValue), m_dataVertical((w+1)*h, initValue) {}

    /*T operator()(int i, int j) const {
        return m_data[i*m_width + j];
    }

    T& operator()(int i, int j) {
        return m_data[i*m_width + j];
    }*/

    T horizontal(int i, int j) const {
        return m_dataHorizontal[i*m_width + j];
    }

    T& horizontal(int i, int j) {
        return m_dataHorizontal[i*m_width + j];
    }

    T vertical(int i, int j) const {
        return m_dataVertical[i*(m_width+1) + j];
    }

    T& vertical(int i, int j) {
        return m_dataVertical[i*(m_width+1) + j];
    }

    const std::vector<T>& rawHorizontal() const {
        return m_dataHorizontal;
    }
    std::vector<T>& rawHorizontal() {
        return m_dataHorizontal;
    }

    const std::vector<T>& rawVertical() const {
        return m_dataHorizontal;
    }

    std::vector<T>& rawVertical() {
        return m_dataHorizontal;
    }

private:
    int m_width;
    int m_height;
    std::vector<T> m_dataHorizontal;
    std::vector<T> m_dataVertical;
};


#endif //EFS_STAGGEREDGRID_H
