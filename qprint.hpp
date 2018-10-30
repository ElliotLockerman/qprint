
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

// I should make a string view class...

// Helper to print a range of chracters to an ostream (so we don't have to substr
//  and potentially make a copy)
// npos is (size_t)-1, so if passed for len will print out untill end of string
static inline void print_range(std::ostream& os, const std::string& s, size_t start, size_t len) {
    assert(start <= s.size());

    for (size_t i = 0; i < len; ++i) {
        if (start+i >= s.size()) { return; }
        os << s[start+i];
    }
}

// start, len apply to lhs only
static inline int compare_range(const std::string& lhs, size_t start, size_t len, const std::string& rhs) {
    assert(start <= lhs.size());

    for (size_t i = 0; i < len; ++i) {

        // Got to the end -> equal
        if (start+i == lhs.size() && i == rhs.size()) {
            return 0;

        // lhs finished first -> lhs is greater
        } else if (start+i == lhs.size()) {
            return 1;

        // lhs finished first -> rhs is greater
        } else if (i == rhs.size()) {
            return -1;
        } 

        auto diff = lhs.at(start+i) - rhs.at(i);
        if (diff != 0) {
            return diff > 0 ? 1 : -1;
        }
    }

    return 0;
}




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








//////////////////////////////////////////////////////////////////////////////////
// qformat
// prints format string (with substituted values) to ss (first argument)
//////////////////////////////////////////////////////////////////////////////////

// Base case: just print string s
static inline void qformat_rec(std::stringstream& ss, const std::string&& s, size_t start) {
    print_range(ss, s, start, std::string::npos);
    assert(s.find('{', start) == std::string::npos && "More {}s than arguments");
}

// Recusive case: print first value and recurse
template<typename T, typename... Ts>
static inline void qformat_rec(std::stringstream& ss, const std::string&& s, size_t start, T&& v, Ts&&...vs) {

    auto open = s.find('{', start);
    assert(open != std::string::npos && "More arguments than {}s");

    auto close = s.find('}', start+1);
    assert(s.find("}") != std::string::npos);

    bool hex = false;
    if (close - open > 1) {
        hex = compare_range(s, open+1, close-open-1, "x") == 0;
        if (!hex) {
            fprintf(stderr, "Unrecognized format string \"");
            print_range(ss, s, open+1, close-open-1);
            fprintf(stderr, "\" at position %zu\n", open+1);
            assert(false);
        }
    }

    print_range(ss, s, start, open-start);

    if (hex) { 
        ss << std::hex; 
        if (HEX_PREFIX) { ss << "0x"; }
    }
    put_to(ss, std::forward<T>(v));
    if (hex) { ss << std::dec; }


    qformat_rec(ss, std::forward<const std::string>(s), close+1, std::forward<Ts>(vs)...);
}




//////////////////////////////////////////////////////////////////////////////////
// qformat and specialization
// uses qformat to print to an ostream, stdout, or stderr
//////////////////////////////////////////////////////////////////////////////////

// Public interface: same as recursive case, but creates ss and returns str
template<typename... Ts>
static inline std::string qformat(const std::string&& s, Ts&&...vs) {
    std::stringstream ss;
    qformat_rec(ss, std::forward<const std::string>(s), 0, std::forward<Ts>(vs)...);
    return ss.str();
}

template<typename... Ts>
static inline void qprint(std::ostream& os, const std::string&& s, Ts&&...vs) {
    std::stringstream ss;
    qformat_rec(ss, std::forward<const std::string>(s), 0, std::forward<Ts>(vs)...);
    os << ss.str();
}

template<typename... Ts>
static inline void qprint(const std::string& s, Ts&&...vs) {
    qprint(std::cout, std::forward<const std::string>(s), std::forward<Ts>(vs)...);
}

template<typename... Ts>
static inline void qerr(const std::string& s, Ts&&...vs) {
    qprint(std::cerr, std::forward<const std::string>(s), std::forward<Ts>(vs)...);
}

} // namespace qprint

