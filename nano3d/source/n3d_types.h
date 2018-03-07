#pragma once
// n3d_types.h
//   internal data types

#include "n3d_util.h"
#include <stdint.h>

// vector type
template <typename type_t, uint32_t size_>
struct n3d_list_t {

    n3d_list_t()
        : head_(0)
    {
    }

    void insert(type_t t)
    {
        n3d_assert(head_ < size_);
        item_[head_++] = t;
    }

    void remove(uint32_t ix)
    {
        n3d_assert(ix < head_);
        item_[ix] = item_[--head_];
    }

    uint32_t size() const
    {
        return head_;
    }

    type_t& get(uint32_t ix)
    {
        n3d_assert(ix < head_);
        return item_[ix];
    }

protected:
    uint32_t head_;
    type_t item_[size_];
};

// validated type
template <typename type_t>
struct valid_t {

    valid_t()
        : valid_(false)
    {
    }

    valid_t(type_t type)
        : valid_(true)
        , type_(type)
    {
    }

    valid_t(const valid_t & rhs)
        : valid_(rhs.valid_)
        , type_(rhs.type_)
    {
    }

    valid_t(valid_t && rhs)
        : valid_(rhs.valid_)
        , type_(rhs.type_)
    {    
    }

    valid_t & operator = (const valid_t & rhs) {
        valid_ = rhs.valid_;
        type_ = rhs.type_;
        return *this;
    }

    operator bool () const {
        return valid_;
    }

    type_t * get() {
        return valid_ ? &type_ : nullptr;
    }

    void reset() {
        valid_ = false;
    }
    
    void reset(type_t & type) {
        valid_ = true;
        type_ = type;
    }

protected:
    bool valid_;
    type_t type_;
};
