

#include "qprint.hpp"


int main() {
    qprint("");
    qprint("test a\n");
    qprint("ten: {}\n", 10);

    auto str = qformat("ten: {}, twenty: {}\n", 10, 20);
    qprint("{}", str);
}

