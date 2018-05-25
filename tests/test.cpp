
#include <vector>

#include "../qprint.hpp"


int main() {

    qprint("");
    qprint("test a\n");
    qprint("ten: {}\n", 10);

    int ten = 10;
    const char* twenty = "twenty";
    auto str = qformat("ten: {}, twenty: {}\n", ten, twenty);
    qprint("{}", str);
    qprint("{}", "string literal\n");

    std::vector<int> v{1,2,3,4};
    qprint("{}\n", v);
}

