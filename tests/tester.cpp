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
        std::println("{}) {}: {}", i + 1, tests[i].description, (passed ? "[PASSED]" : "[FAILED]"));
    }
    float percentage = (float)passed_cnt / (float)tests_cnt * 100;
    std::println("Group results: {}/{} | {:.2f}%\n", passed_cnt, tests_cnt, percentage);
    std::fflush(stdout);
    return passed_cnt == tests_cnt;
}
