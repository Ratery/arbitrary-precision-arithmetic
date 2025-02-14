#include "LongNum.hpp"
#include <iostream>
#include <stdexcept>

LongNum calculate_pi(const unsigned precision) {
    // source: https://www.craig-wood.com/nick/articles/pi-chudnovsky/
    LongNum k = 1;
    LongNum a_k = (1_longnum).with_precision(precision);
    LongNum a_sum = (1_longnum).with_precision(precision);
    LongNum b_sum = 0;
    const LongNum C = 640320;
    const LongNum C3_OVER_24 = C.pow(3) / 24;
    while (a_k != 0) {
        a_k *= -(6 * k - 5) * (2 * k - 1) * (6 * k - 1);
        a_k /= k.pow(3) * C3_OVER_24;
        a_sum += a_k;
        b_sum += k * a_k;
        k += 1;
    }
    LongNum total = a_sum * 13591409 + b_sum * 545140134;
    LongNum pi = (426880 * (10005_longnum).with_precision(precision).sqrt()) / total;
    return pi;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Specify calculation precision by providing a command line argument\n";
        return 1;
    }
    const std::string arg = argv[1];
    try {
        const int precision = std::stoi(arg);
        if (precision <= 0) {
            std::cerr << "Precision must be a positive number\n";
            return 1;
        }
        const LongNum pi = calculate_pi(std::max(380u, (unsigned)precision * 69 / 20 + 1));
        std::cout << pi.to_string(precision) << '\n';
        return 0;
    } catch (const std::invalid_argument& ex) {
        std::cerr << "Invalid precision: " << arg << '\n';
    } catch (const std::out_of_range& ex) {
        std::cerr << "Precision out of range: " << arg << '\n';
    }
}
