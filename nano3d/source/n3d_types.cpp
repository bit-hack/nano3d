#include "n3d_types.h"

#include "nano3d.h"

struct n3d_triangle_stack {

    typedef n3d_rasterizer_t::triangle_t triangle_t;

    static const uint32_t size_ = 1024 * 64;

    triangle_t triangle_[size_];
    uint32_t head_;

    triangle_t * next() {
        assert(head_ < size_);
        return triangle_ + head_++;
    }

    void clear() {
        head_;
    }
};
