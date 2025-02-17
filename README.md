# LongNum arbitrary-precision library

First-year educational project at HSE FCS SE. The library supports handling fixed-point arithmetic
of arbitrary precision.

The repository includes tests and a sample program that calculates pi with a given precision
for performance evaluating.


## Requirements

Requires GCC 14 or newer.


## Makefile

**Main targets:**
* `make pi PRECISION=100` — run sample program calculating 100 places of pi
* `make test` — test the library

Prefix make with `RELEASE=1` to use release parameters (**dramatically** faster).


## Usage

### General info

The library introduces a `LongNum` class that implements fixed-point arithmetic (fractional part has fixed size,
total number size is unlimited) with selectable binary precision.

### Creating an instance

**It is possible to create a `LongNum` instance in different ways:**

* From `long long` or `unsigned long long` (then precision will be set to `0`):
```c++
LongNum x(25LL);
LongNum y = 25;
```

* From decimal or binary string representation:
```c++
auto x = LongNum::from_string("5.32", 10);  // specifying precision (optional), else it sets automatically
auto y = LongNum::from_binary_string("-1101.01001");  // precision = number of chars after '.'
```

* From integer, string, or double literal using `_longnum` suffix:
```c++
-125_longnum  // precision = 0 (integer)
3.1415_longnum  // precision is set automatically
"3.1415"_longnum  // the same as LongNum::from_string()
```

> [!IMPORTANT]  
> When creating `LongNum` instance from a **fractional number** if high precision is required,
> use `LongNum::from_string()` providing binary precision as a second argument.
> Else the precision of the result will be selected automatically, that may lead to unintended **precision loss**.  
> *See the example below.*

```c++
// 0.1 = 0.0001100110011... so it can't be represented exaclty in binary

// the right way
auto x = LongNum::from_string("0.1", 128);  // specifying precision
std::cout << "x = " << x << '\n';
// >>> x = 0.09999999999999999999999999999999999999823...

// loosing precision
auto y = LongNum::from_string("0.1");  // same as "0.1"_longnum and 0.1_longnum
std::cout << "y = " << y << '\n';  // y.set_precision(128) can not help, because the "data" was lost in the first step
// >>> y = 0.09999999999999999996747393482543486697977...
```

### Controlling the precision
Since numbers are stored in binary, precision is specified as the number of **binary** places after the point.

There are two methods to control the precision of numbers:
* `LongNum::set_precision(precision)` sets the specified precision
* `LongNum::with_precision(precision)` returns a copy of `LongNum` instance with the specified precision

When decreasing the precision, some of the places that no longer fit are simply cut off.

Example:
```c++
// 0.125 = 0.001 (binary)

std::cout << (0.125_longnum).with_precision(3) << '\n';
// >>> 0.125

std::cout << (0.125_longnum).with_precision(2) << '\n';  // 0.001 -> 0.00
// >>> 0
```

### I/O operators
Input and output operators `>>` and `<<` are overloaded for the `LongNum` class and
allow to input/output numbers in decimal form.

Example:
```c++
LongNum x;
std::cout << "Enter a number: ";
std::cin >> x;
std::cout << x << '\n;
```
```
>>> Enter a number: -20.25
>>> The number is: -20.25
```

Also, there are methods to convert `LongNum` to string:
```c++
auto x = LongNum::from_string("1234.56789");  // creating an instance

std::string str = x.to_string();  // converting to a decimal string representation
std::string binary_str = x.to_binary_string();
```

> [!NOTE]  
> For faster conversion of extremely large numbers `LongNum::to_string()` takes an optional argument
> specifying the maximum number of decimal places after the point.

### Relational operators
Relational operators `==`, `!=`, `>`, `>=`, `<`, `<=` are overloaded for the `LongNum` class.

### Mathematical operators
Operators `+`, `-`, `*`, `/`, `<<`, `>>` as well as operators `+=`, `-=`, `*=`, `/=` are overloaded for the `LongNum`
class just like you would expect them to.  
This makes operations with its instances the same as with built-in numeric types.

> [!IMPORTANT]  
> The precision of the result of binary arithmetic operation is set to the **maximum** of operands' precision.

> [!NOTE]  
> To improve performance due to implementation details it is recommended to choose precision to be a multiple of **32**
> and, if possible, perform `+` and `-` operations on numbers of the same precision.

Some other mathematical functions:
* `LongNum::pow(x)` returns a number raised to the power `x`
* `LongNum::sqrt()` calculates square root of a number
* `LongNum::truncate()` returns the whole part of a number
* `LongNum::frac()` returns the fractional part of a number

See the [header file](https://github.com/Ratery/arbitrary-precision-arithmetic/blob/main/src/longnum.hpp)
for more details.
