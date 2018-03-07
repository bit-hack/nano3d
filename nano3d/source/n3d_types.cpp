#include <array>

#include "n3d_types.h"
#include "nano3d.h"

struct n3d_triangle_stack {

    typedef n3d_rasterizer_t::triangle_t triangle_t;

    std::array<triangle_t, 1024 * 64> triangle_;
    uint32_t head_;

    n3d_triangle_stack()
        : head_(0)
    {
    }

    triangle_t& next()
    {
        n3d_assert(head_ < triangle_.size());
        return triangle_[head_];
        ++head_;
    }

    void clear()
    {
        head_ = 0;
    }
};
