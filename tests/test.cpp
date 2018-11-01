
#include <vector>
#include <unordered_map>
#include <cstdint>

#include "../qprint.hpp"

using namespace qp;

int main() {

    qprint("");
    qprint("test a\n");
    qprint("ten: {}\n", 10);
    qprint("hex: {x}\n", 10);

    int ten = 10;
    const char* twenty = "twenty";
    auto str = qformat("ten: {}, twenty: {}\n", ten, twenty);
    qprint("{}", str);
    qprint("{}", "string literal\n");

    std::vector<int> v{1,2,3,4};
    qprint("{}\n", v);

    std::unordered_map<char, int> map{{'a', 1}, {'b', 2}, {'c', 3}};
    qprint("{}\n", map);


    std::unordered_map<char, std::vector<int>> m2{{'a', {1,2}}, {'b', {3,4}}, {'c', {5,6}}};
    qprint("{}\n", m2);

}

