
#pragma once

#include <sstream>
#include <iostream>
#include <string>
#include <cassert>
#include <string>

namespace {
const char* qprint_delim = "{}";
const size_t qprint_delim_len = strlen(qprint_delim);
} // anon namespace


// Helper to print a range of chracters to an ostream (so we don't have to substr
//  and potentially make a copy)
// npos is (size_t)-1, so if passed for len will print out untill end of stirng
static inline void print_range(std::ostream& os, const std::string& s, size_t start, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (start+i >= s.size()) { return; }
        os << s[start+i];
    }
}



//////////////////////////////////////////////////////////////////////////////////
// qformat
// prints format string (with substituted values) to ss (first argument)
//////////////////////////////////////////////////////////////////////////////////

// Base case: just print string s
static inline void qformat_rec(std::stringstream& ss, const std::string& s, size_t start) {
    print_range(ss, s, start, std::string::npos);
    assert(s.find(qprint_delim, start) == std::string::npos && "More {}s than arguments");
}

// Recusive case: print first value and recurse
template<typename T, typename... Ts>
static inline void qformat_rec(std::stringstream& ss, const std::string& s, size_t start, T v, Ts...vs) {
    auto found = s.find(qprint_delim, start);
    assert(found != std::string::npos && "More arguments than {}s");
    print_range(ss, s, start, found-start);
    ss << v;
    qformat_rec(ss, s, found+qprint_delim_len, vs...);
}

// Public interface: same as recursive case, but creates ss and returns str
template<typename... Ts>
static inline std::string qformat(const std::string& s, Ts...vs) {
    std::stringstream ss;
    qformat_rec(ss, s, 0, vs...);
    return ss.str();
}



//////////////////////////////////////////////////////////////////////////////////
// qformat and specialization
// uses qformat to print to an ostream, stdout, or stderr
//////////////////////////////////////////////////////////////////////////////////
template<typename... Ts>
static inline void qprint(std::ostream& os, const std::string& s, Ts...vs) {
    std::stringstream ss;
    qformat_rec(ss, s, 0, vs...);
    os << ss.str();
}

template<typename... Ts>
static inline void qprint(const std::string& s, Ts...vs) {
    qprint(std::cout, s, vs...);
}

template<typename... Ts>
static inline void qerr(const std::string& s, Ts...vs) {
    qprint(std::cerr, s, vs...);
}


