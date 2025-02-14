#ifndef TESTER_HPP
#define TESTER_HPP

#include <vector>
#include <functional>
#include <string>

class Tester {
    struct Test {
        std::function<bool()> function;
        std::string description;
    };

    std::string group_name;
    std::vector<Test> tests;

public:
    explicit Tester(std::string name);
    void add_test(std::function<bool()> function, std::string description);
    bool run_tests();
};

#endif //TESTER_HPP
