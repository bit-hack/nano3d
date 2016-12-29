#include <cstdio>
#include <stdint.h>

extern bool test1();
extern bool test2();

typedef bool (*test_t)();

struct test_cast_t {
    test_t test_;
    const char * name_;
};

test_cast_t tests[] = {
    { test1,  "test1" },
    { test2,  "test2" },
    { nullptr, nullptr }
};

int main(int argc, char ** args) {

    uint32_t total = 0, passing = 0;

    // for each test case
    for (test_cast_t * t = tests; t->test_; ++t, ++total) {

        printf("%s> ", t->name_);

        if (t->test_()) {
            printf("passed");
            ++passing;
        }
        else {
            printf("failed");
        }

        printf("\n");
    }

    printf("%d of %d passing\n", passing, total);
    getchar();

    return 0;
}
