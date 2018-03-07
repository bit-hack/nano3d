#include <cstdio>
#include <stdint.h>

extern bool thread_test_1();
extern bool thread_test_2();

typedef bool (*test_t)();

struct test_cast_t {
    test_t test_;
    const char* name_;
};

test_cast_t tests[] = {
    { thread_test_1, "thread test 1" },
    { thread_test_2, "thread test 2" },
    { nullptr, nullptr }
};

int main(int argc, char** args)
{
    uint32_t total = 0, passing = 0;

    // for each test case
    for (test_cast_t *t = tests; t->test_; ++t, ++total) {

        printf("%s> ", t->name_);

        if (t->test_()) {
            printf("passed");
            ++passing;
        } else {
            printf("failed");
        }

        printf("\n");
    }

    printf("%d of %d passing\n", passing, total);
    getchar();

    return 0;
}
