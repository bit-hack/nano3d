#pragma once

#include <stdint.h>
#include <stdio.h>

struct reader_t {

    reader_t();
    ~reader_t();

    // open a file
    bool open(const char * path);

    // close a file
    void close();

    // check if file is open
    bool is_open();

    // return file size
    uint32_t size();

    // read raw byte length
    bool read(void * dst, uint32_t num);

    // read arbitrary data
    template <typename type_t>
    bool read(type_t & out) {
        return read(&out, uint32_t(sizeof(type_t)));
    }

    // read array
    template <typename type_t, uint32_t size>
    bool read(type_t (&out)[size]) {
        for (int i=0; i<size; ++i)
            if (! read(out[i]))
                return false;
        return true;
    };

    // absolute seek
    bool seek(uint32_t offset);

    // relative seek
    bool skip(int32_t relative);

protected:

    FILE * fd_;
};
