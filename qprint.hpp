
#pragma once

#include <sstream>
#include <iostream>
#include <string>
#include <cassert>
#include <string>
#include <cstring>
#include <iterator>
#include <utility>


namespace qp {


const bool HEX_PREFIX = true;

//////////////////////////////////////////////////////////////////////////////////
// Support
//////////////////////////////////////////////////////////////////////////////////


class StringView {
public:
    StringView() = default;

    StringView(const char* str) : start(str), end(start + strlen(str)) {}
    StringView(const std::string& str) : start(str.c_str()), 
        end(start + str.size()) {}


    StringView(const char* str, size_t off, size_t len=npos) 
    : start(str + off), end(start + len) {
        if (len == npos) {
            end = str + strlen(str);
        } else {
            assert(len <= strlen(start) - off);
        }
    }

    StringView(const std::string& str, size_t off, size_t len=npos)
    : start(str.c_str()+off), end(start + len) {
        if (len == npos) {
            end = str.c_str() + str.size();
        } else {
            assert(len <= str.size() - off);
        }
    }


    StringView(const StringView&) = default;

    StringView(const StringView sv, size_t off, size_t len=npos) {
        *this = sv.substr(off, len);
    }

    StringView substr(size_t off, size_t len=npos) const {
        assert(start + off + len <= end);
        return StringView(start, off, len);
    }

    size_t size() const { return (size_t)(end - start); }

    bool operator==(const StringView& rhs) const {
        return this->compare(rhs) == 0;
    }

    bool operator!=(const StringView& rhs) const {
        return this->compare(rhs) != 0;
    }

    int compare(const StringView& rhs) const {
        for (size_t i = 0; i <= size(); ++i) {

            // Got to the end of both -> equal
            if (i == this->size() && i == rhs.size()) {
                return 0;

            // lhs finished first -> lhs is greater
            } else if (i == this->size()) {
                return 1;

            // lhs finished first -> rhs is greater
            } else if (i == rhs.size()) {
                return -1;
            } 

            auto diff = this->at(i) - rhs[i];
            if (diff != 0) {
                return diff;
            }
        }

        assert(false && "Unreachable");
    }

    // I should write something more general, but this is enough for now...
    size_t find(char ch, size_t off=0) const {
        auto* p = start + off;
        while (p != end) {
            if (*p == ch) { return (size_t)(p - start); }
            ++p;
        }
        return npos;
    }

    char operator[](size_t idx) const {
        assert(start + idx < end);
        return *(start + idx);
    }

    char at(size_t idx) const { return (*this)[idx]; }

    std::string str() const { return std::string(start, end); }


    friend bool operator<(const StringView& lhs, const StringView& rhs) {
        return lhs.compare(rhs) < 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const StringView& sv) {
        auto* p = sv.start;
        while (p != sv.end) {
            os << *p;
            ++p;
        }

        return os;
    }

    static const size_t npos = (size_t)-1;


private:
    const char* start = nullptr;
    const char* end = nullptr;
};



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



// detect an iterable type
// https://stackoverflow.com/a/29634934
using std::begin;
using std::end;

template <typename T>
auto constexpr is_iterable_impl(int)
-> decltype (
    begin(std::declval<T&>()) != end(std::declval<T&>()), // begin/end and operator !=
    void(), // Handle evil operator ,
    ++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
    void(*begin(std::declval<T&>())), // operator*
    std::true_type{});

template<typename T>
std::false_type constexpr is_iterable_impl(...);

template <typename T>
using is_iterable = decltype(is_iterable_impl<T>(0));



// detect a printable type
template<typename T>
auto constexpr is_printable_impl(int)
-> decltype(
    std::declval<std::ostream&>() << std::declval<T>(),
    std::true_type{});


template<typename T>
std::false_type constexpr is_printable_impl(...);

template <typename T>
using is_printable = decltype(is_printable_impl<T>(0));




// regular case
template<typename T>
typename std::enable_if<!is_iterable<T>::value || is_printable<T>::value, void>::type 
put_to(std::ostream& os, T&& v) { os << v; }


template<typename T1, typename T2>
void put_to(std::ostream& os, std::pair<T1,T2>& v);

// iterable case
template<typename T>
typename std::enable_if<is_iterable<T>::value && !is_printable<T>::value, void>::type 
put_to(std::ostream& os, T&& v) {
    os << "{";

    auto it = std::begin(v);
    if (it != std::end(v)) { put_to(os, *it); }
    while (++it != std::end(v)) { 
        os << ", "; 
        put_to(os, *it);
    }

    os << "}";
}

// pair case
template<typename T1, typename T2>
void put_to(std::ostream& os, std::pair<T1,T2>& v) { 
    os << "(";
    put_to(os, v.first);
    os << ", ";
    put_to(os, v.second);
    os << ")";  
}



////////////////////////////////////////////////////////////////////////////////
// qformat
// prints format string (with substituted values) to ss (first argument)
//////////////////////////////////////////////////////////////////////////////////

// Base case: just print string s
static inline void qformat_rec(std::stringstream& ss, const StringView s) {
    ss << s;
    assert(s.find('{') == std::string::npos && "More {}s than arguments");
}

// Recusive case: print first value and recurse
template<typename T, typename... Ts>
static inline void qformat_rec(std::stringstream& ss, const StringView s, T&& v, Ts&&...vs) {

    auto open = s.find('{');
    assert(open != StringView::npos && "More arguments than {}s");

    auto close = s.find('}', open+1);
    assert(close != std::string::npos);

    auto format = s.substr(open+1, close-(open+1));
    bool hex = false;
    if (close - open > 1) {
        hex = format == "x";
        if (!hex) {
            fprintf(stderr, "Unrecognized format string \"");
            ss << format;
            fprintf(stderr, "\" at position %zu\n", open+1);
            assert(false);
        }
    }

    ss << StringView(s, 0, open);

    if (hex) { 
        ss << std::hex; 
        if (HEX_PREFIX) { ss << "0x"; }
    }
    put_to(ss, std::forward<T>(v));
    if (hex) { ss << std::dec; }


    qformat_rec(ss, s.substr(close+1), std::forward<Ts>(vs)...);
}






template<typename... Ts>
static inline std::string qformat_impl(const std::string&& s, Ts&&...vs) {
    std::stringstream ss;
    qformat_rec(ss, StringView(s), std::forward<Ts>(vs)...);
    return ss.str();
}

template<typename... Ts>
static inline void qprint_os_impl(std::ostream& os, const std::string&& s, Ts&&...vs) {
    std::stringstream ss;
    qformat_rec(ss, StringView(s), std::forward<Ts>(vs)...);
    os << ss.str();
}

template<typename... Ts>
static inline void qprint_impl(const std::string& s, Ts&&...vs) {
    qprint_os_impl(std::cout, std::forward<const std::string>(s), std::forward<Ts>(vs)...);
}

template<typename... Ts>
static inline void qerr_impl(const std::string& s, Ts&&...vs) {
    qprint_os_impl(std::cerr, std::forward<const std::string>(s), std::forward<Ts>(vs)...);
}

} // namespace qp





////////////////////////////////////////////////////////////////////////////////
// qformat and specialization
// uses qformat to print to an ostream, stdout, or stderr
//////////////////////////////////////////////////////////////////////////////////

// https://stackoverflow.com/a/33349105
#define qp_check_args(s, ...) static_assert(qp::ConstStr(s).count_placeholders() == std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value, "Number of values does not match number of placeholders")

#define qformat(s, ...) ({ \
    qp_check_args(s, ##__VA_ARGS__); \
    qp::qformat_impl(s, ##__VA_ARGS__); \
})

#define qprint_os(os, s, ...) ({ \
    qp_check_args(s, ##__VA_ARGS__); \
    qp::qprint_os_impl(os, s, ##__VA_ARGS__); \
} while(0)

#define qprint(s, ...) do { \
    qp_check_args(s, ##__VA_ARGS__); \
    qp::qprint_impl(s, ##__VA_ARGS__); \
} while(0)

#define qerr(s, ...) do { \
    qp_check_args(s, ##__VA_ARGS__); \
    qp::qerr_impl(s, ##__VA_ARGS__); \
} while(0)



