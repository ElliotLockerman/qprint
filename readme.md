
# QPrint

A simple header-only C++ printing library inspired by Python's `str.format`.

Uses `operator<<` internally, with built-in overloads for aggregates.

Example:

``` C++
qprint("ten: {}\n", 10);  // ten: 10
qprint("ten: {x}\n", 10); // ten: 0xa

std::unordered_map<char, int> map{{'a', 1}, {'b', 2}, {'c', 3}};
std::string = qformat("{}\n", map); // string == "{(c, 3), (b, 2), (a, 1)}"
```

qprint supports a limited (but growing) format language.

Available options:

- `{x}`: print int as hex
- `{o}`: print int as octal
- `{f}`: print float as fixed (i.e., not scientific)
- `{e}`: print float as exponential (i.e., scientific)
- `{b}`: print bool with `true` or `false` rather than `1` or `0`

These work by appying the standard C++ IO manipulators and have the same sematics, excepting that the stream state is saved before the manipulation and reverted afterwords. 

Because `operator<<` is used internally, other IO manipulation can be acheived by "printing" the operator. For example, to print a number with a minimum width of 20, one could use

``` C++
qprint("{}{}\n", std::setw(20), 100); //                 100
                                      //\_______ _______/
                                      //        v
                                      //     17 spaces
```

## Todo

- More format options
- Better testing


