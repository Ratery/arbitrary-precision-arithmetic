#include "tester.hpp"
#include <print>
#include <utility>

Tester::Tester(std::string name) : group_name(std::move(name)) {}

void Tester::add_test(std::function<bool()> function, std::string description) {
    tests.push_back({std::move(function), std::move(description)});
}

bool Tester::run_tests() {
    if (tests.empty()) {
        return true;
    }
    std::println("========== Running [ {} ] ==========", group_name);
    unsigned tests_cnt = tests.size(), passed_cnt = 0;
    for (size_t i = 0; i < tests.size(); i++) {
        const bool passed = tests[i].function();
        passed_cnt += passed;
        std::println(
            "{}) {}: {}", i + 1,
            tests[i].description,
            (passed ? "\033[1;32m[PASSED]\033[0m" : "\033[1;31m[FAILED]\033[0m")
        );
    }

    float percentage = (float)passed_cnt / (float)tests_cnt * 100;
    const bool success = (passed_cnt == tests_cnt);
    std::println(
        "\033[1;{}mGroup results: {}/{} | {:.2f}%\033[0m\n",
        (success ? 32 : 31), passed_cnt, tests_cnt, percentage
    );
    std::fflush(stdout);
    return success;
}
