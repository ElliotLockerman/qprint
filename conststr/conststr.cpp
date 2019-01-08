
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <tuple>
#include <string>

#include "../qprint.hpp"

// based on https://akrzemi1.wordpress.com/2011/05/11/parsing-strings-at-compile-time-part-i/
class ConstStr {
public:

    template <unsigned long long N>
    constexpr ConstStr(const char (&arr) [N]) : start(arr), thesize(N - 1) {
        static_assert( N >= 1, "not a string literal");
    }

    constexpr char operator[](size_t i) const { 
        return requires_inrange(i, thesize), start[i]; 
    }

    constexpr char at(size_t i) const { return (*this)[i]; }

    constexpr unsigned size() const { return thesize; }

    constexpr size_t count_placeholders(size_t i=0, size_t num=0, bool in=false) const {
        return 
            i == thesize
                ? num 
                : this->at(i) == '{'
                    ? requires_true(!in), count_placeholders(i+1, num+1, true)
                    : this->at(i) == '}' 
                        ? requires_true(in), count_placeholders(i+1, num, false)
                        : count_placeholders(i+1, num, in);
            
           
    }

    operator std::string() const { return std::string(start); }

private:

    const char* start = nullptr;
    size_t thesize = -1;

    static constexpr size_t requires_inrange(size_t i, size_t len) {
        return i >= len ? throw std::out_of_range("") : i;
    }

    static constexpr bool requires_true(bool b) {
        return b ? b : throw std::exception();
    }

};


// https://stackoverflow.com/a/33349105
#define qp_check_args(s, ...) static_assert(ConstStr(s).count_placeholders() == std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value, "Number of values does not match number of placeholders")

#define qprint(s, ...) do { \
    qp_check_args(s, ##__VA_ARGS__); \
    qp::qprint(s, ##__VA_ARGS__); \
} while(0)

#define qerr(s, ...) do { \
    qp_check_args(s, ##__VA_ARGS__); \
    qp::qerr(s, ##__VA_ARGS__); \
} while(0)

#define qformat(s, ...) do { \
    qp_check_args(s, ##__VA_ARGS__); \
    qp::qformat(s, ##__VA_ARGS__); \
} while(0)


int main() {
    qprint("test\n");
    qprint("{} {}\n", 1, 2);
}






