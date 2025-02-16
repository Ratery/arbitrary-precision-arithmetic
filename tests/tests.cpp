#include "tester.hpp"
#include "../src/longnum.hpp"
#include <iostream>
#include <stdexcept>

template<typename T>
std::function<bool()> is_equal(const T& lhs, const T& rhs) {
    return [lhs, rhs]() { return lhs == rhs; };
}

bool test_integer_conversions() {
    Tester tester("Integer conversions");
    tester.add_test(
        is_equal(LongNum::from_string("0").to_string(), std::string("0")),
        "From int 0"
    );
    tester.add_test(
        is_equal(LongNum::from_string("25178157").to_string(), std::string("25178157")),
        "From int 25178157"
    );
    tester.add_test(
        is_equal(LongNum::from_string("91378721617823").to_string(), std::string("91378721617823")),
        "From int 91378721617823"
    );
    return tester.run_tests();
}

bool test_shifts() {
    Tester tester("Bit shifts");
    tester.add_test(
        is_equal(1_longnum << 1, 2_longnum),
        "1 << 1"
    );
    tester.add_test(
        is_equal(1_longnum << 32, LongNum(1ll << 32)),
        "1 << 32 (new limb)"
    );
    tester.add_test(
        is_equal(1_longnum >> 1,  0_longnum),
        "1 >> 1"
    );
    return tester.run_tests();
}

bool test_comparison() {
    Tester tester("Operator *");
    tester.add_test(
        []() { return 0_longnum == -(0_longnum); },
        "0 = -0"
    );
    tester.add_test(
        []() { return 1_longnum != -(1_longnum); },
        "0 != -1"
    );
    tester.add_test(
        []() { return 1_longnum > 0_longnum; },
        "1 > 0"
    );
    tester.add_test(
        []() { return -(1_longnum) < 0_longnum; },
        "-1 < 0"
    );
    tester.add_test(
        []() { return -(1_longnum) < 1_longnum; },
        "-1 < 1"
    );
    tester.add_test(
        []() { return 1_longnum > -(1_longnum); },
        "1 > -1"
    );
    tester.add_test(
        []() { return 1_longnum <= 1_longnum; },
        "1 <= 1"
    );
    tester.add_test(
        []() { return "0.000000000000000000000000000000001"_longnum < "0.00000000000000000000000000000001"_longnum; },
        "Multiple limbs, different digits after point"
    );
    tester.add_test(
        []() { return (0.5_longnum).with_precision(3) == (0.5_longnum).with_precision(10); },
        "Equal numbers with different precision"
    );
    tester.add_test(
        []() { return (0.35_longnum).with_precision(3) == (0.3_longnum).with_precision(3); },
        "Different numbers with precision drop are equal"
    );
    return tester.run_tests();
}

bool test_addition_subtraction() {
    Tester tester("Operators +, -");
    tester.add_test(
        is_equal(2_longnum + 0_longnum, 2_longnum),
        "x + 0 = x"
    );
    tester.add_test(
        is_equal(0_longnum + 2_longnum, 2_longnum),
        "0 + x = x"
    );
    tester.add_test(
        is_equal(2_longnum - 0_longnum, 2_longnum),
        "x - 0 = x"
    );
    tester.add_test(
        is_equal(0_longnum - 2_longnum, -2_longnum),
        "0 - x = x"
    );
    tester.add_test(
        is_equal(2_longnum - 2_longnum, 0_longnum),
        "x - x = 0"
    );
    tester.add_test(
        is_equal(0.25_longnum + 0.5_longnum, 0.75_longnum),
        "0.25 + 0.5 = 0.75"
    );
    tester.add_test(
        is_equal(0.25_longnum - 0.5_longnum, -0.25_longnum),
        "0.25 - 0.5 = -0.25"
    );
    tester.add_test(
        is_equal(-0.25_longnum - 0.5_longnum, -0.75_longnum),
        "-0.25 - 0.5 = -0.75"
    );
    return tester.run_tests();
}

bool test_multiplication() {
    Tester tester("Operator *");
    tester.add_test(
        is_equal(2_longnum * 0_longnum, 0_longnum),
        "x * 0 = 0"
    );
    tester.add_test(
        is_equal(0_longnum * 2_longnum, 0_longnum),
        "0 * x = 0"
    );
    tester.add_test(
        is_equal(2_longnum * 1_longnum, 2_longnum),
        "x * 1 = x"
    );
    tester.add_test(
        is_equal(1_longnum * 2_longnum, 2_longnum),
        "1 * x = x"
    );
    tester.add_test(
        is_equal(-1_longnum * 1_longnum, -1_longnum),
        "-1 * 1 = 1"
    );
    tester.add_test(
        is_equal(1_longnum * -1_longnum, -1_longnum),
        "1 * -1 = -1"
    );
    tester.add_test(
        is_equal(-1_longnum * -1_longnum, 1_longnum),
        "-1 * -1 = 1"
    );
    tester.add_test(
        is_equal((0.25_longnum).with_precision(4) * 0.25_longnum, 0.0625_longnum),
        "0.25 * 0.25 = 0.625 (with precision = 4)"
    );
    tester.add_test(
        is_equal((0.25_longnum).with_precision(3) * (0.25_longnum).with_precision(3), 0_longnum),
        "0.25 * 0.25 = 0 (with precision = 3)"
    );
    LongNum x = LongNum::from_binary_string("10110110010010011010100110000001.01100000000111001110101100101");
    LongNum y = LongNum::from_binary_string("111011111100010101011100001000101101000.100111000101010111100000111");
    tester.add_test(
        is_equal(x.with_precision(56) * y, LongNum::from_binary_string("10101010101110110100110110001010011111001111011101110101010100010101010.00101111110110010011111001110011110110110101100011000011")),
        "Long multiplication"
    );
    return tester.run_tests();
}

bool test_division() {
    Tester tester("Operator *");
    tester.add_test(
        is_equal(2_longnum / 1_longnum, 2_longnum),
        "x / 1 = x"
    );
    tester.add_test(
        is_equal(2_longnum / -1_longnum, -2_longnum),
        "x / -1 = -x"
    );
    tester.add_test(
        is_equal(((22_longnum).with_precision(32) / 7_longnum).to_binary_string(), std::string("11.00100100100100100100100100100100")),
        "22 / 7 (with precision = 32)"
    );
    return tester.run_tests();
}

int main() {
    bool success = true;
    success &= test_integer_conversions();
    success &= test_shifts();
    success &= test_comparison();
    success &= test_addition_subtraction();
    success &= test_multiplication();
    success &= test_division();
    if (!success) {
        throw std::logic_error("\033[1;31mANY TESTS FAILED\033[0m");
    }
    std::cout << "\033[1;32mALL TESTS PASSED\033[0m" << std::endl;
    return 0;
}
