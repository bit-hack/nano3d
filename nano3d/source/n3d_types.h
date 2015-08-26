#pragma once

#include <stdint.h>
#include "n3d_util.h"

template <typename type_t, uint32_t size_>
struct n3d_list_t {

    n3d_list_t()
        : head_(0)
    {
    }

    void insert(type_t t) {
        assert(head_ < size_);
        item_[head_++] = t;
    }

    void remove(uint32_t ix) {
        assert(ix < head_);
        item_[ix] = item_[--head_];
    }

    uint32_t size() const {
        return head_;
    }

    type_t & get(uint32_t ix) {
        assert(ix < head_);
        return item_[ix];
    }

protected:
    uint32_t head_;
    type_t item_[size_];
};
