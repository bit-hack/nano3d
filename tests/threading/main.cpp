int main(int argc, char ** args) {

    extern bool test1();
    test1();

    extern bool test2();
    for (int i = 0; i < 100; i++)
        test2();

    return 0;
}
