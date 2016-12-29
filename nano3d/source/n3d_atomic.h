#pragma once

#if defined (_MSC_VER)
#include <intrin.h>
#endif

typedef volatile long n3d_atomic_t;

namespace {

#if defined(_MSC_VER)

    long n3d_atomic_inc(n3d_atomic_t &v) {
        return _InterlockedIncrement(&v);
    }

    long n3d_atomic_dec(n3d_atomic_t &v) {
        return _InterlockedDecrement(&v);
    }

    long n3d_atomic_xchg(n3d_atomic_t &v, long x) {
        return _InterlockedExchange(&v, x);
    }

#else

    long n3d_atomic_inc(n3d_atomic_t &v) {
        return __sync_fetch_and_add(&v, 1);
    }

    long n3d_atomic_dec(n3d_atomic_t &v) {
        return __sync_fetch_and_sub(&v, 1);
    }

    long n3d_atomic_xchg(n3d_atomic_t &v, long x) {
        long o = __sync_lock_test_and_set(&v, x);
        __sync_synchronize();
        return o;
    }

#endif

} // namespace {}
