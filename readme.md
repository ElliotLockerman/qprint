
# QPrint

A simple header-only C++ printing library inspired by Python's `str.format`.

Uses `operator<<` internally, with built-in overloads for aggregates.

Example:

```C++
using namespace qp;

qprint("ten: {}\n", 10);  // ten: 10
qprint("ten: {x}\n", 10); // ten: 0xa

std::unordered_map<char, int> map{{'a', 1}, {'b', 2}, {'c', 3}};
std::string = qformat("{}\n", map); // string == "{(c, 3), (b, 2), (a, 1)}"
```

More info coming soon.

## Todo

- String views instead of copying (where appropriate)
- Command line option to format into stringstream before printing
- More format options
- Better testing


