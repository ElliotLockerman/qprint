
#pragma once

#include <sstream>
#include <iostream>
#include <string>
#include <cassert>
#include <string>
#include <cstring>
#include <iterator>
#include <utility>


// Support
namespace {

const char* qprint_delim = "{}";
const size_t qprint_delim_len = strlen(qprint_delim);


// Helper to print a range of chracters to an ostream (so we don't have to substr
//  and potentially make a copy)
// npos is (size_t)-1, so if passed for len will print out untill end of stirng
static inline void print_range(std::ostream& os, const std::string& s, size_t start, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (start+i >= s.size()) { return; }
        os << s[start+i];
    }
}


// detect an iterable variable
// https://stackoverflow.com/a/29634934
using std::begin;
using std::end;

template <typename T>
auto is_iterable_impl(int)
-> decltype (
    begin(std::declval<T&>()) != end(std::declval<T&>()), // begin/end and operator !=
    void(), // Handle evil operator ,
    ++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
    void(*begin(std::declval<T&>())), // operator*
    std::true_type{});

template <typename T>
std::false_type is_iterable_impl(...);

template <typename T>
using is_iterable = decltype(is_iterable_impl<T>(0));



static void put_to(std::ostream& os, std::string& v) { os << v; }
static void put_to(std::ostream& os, const char* v) { os << v; }

template<typename T1, typename T2>
void put_to(std::ostream& os, std::pair<T1,T2>& v) { 
    os << "(" << v.first << ", " << v.second << ")"; 
}

template<typename T>
typename std::enable_if<is_iterable<T>::value != true, void>::type 
put_to(std::ostream& os, T&& v) { os << v; }

template<typename T>
typename std::enable_if<is_iterable<T>::value, void>::type 
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




} // anon namespace




//////////////////////////////////////////////////////////////////////////////////
// qformat
// prints format string (with substituted values) to ss (first argument)
//////////////////////////////////////////////////////////////////////////////////

// Base case: just print string s
static inline void qformat_rec(std::stringstream& ss, const std::string&& s, size_t start) {
    print_range(ss, s, start, std::string::npos);
    assert(s.find(qprint_delim, start) == std::string::npos && "More {}s than arguments");
}

// Recusive case: print first value and recurse
template<typename T, typename... Ts>
static inline void qformat_rec(std::stringstream& ss, const std::string&& s, size_t start, T&& v, Ts&&...vs) {
    auto found = s.find(qprint_delim, start);
    assert(found != std::string::npos && "More arguments than {}s");
    print_range(ss, s, start, found-start);
    put_to(ss, std::forward<T>(v));
    qformat_rec(ss, std::forward<const std::string>(s), found+qprint_delim_len, std::forward<Ts>(vs)...);
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


