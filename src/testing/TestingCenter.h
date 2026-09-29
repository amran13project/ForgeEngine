#pragma once
#include <functional>
#include <string>
#include <vector>
namespace forge::testing {struct TestResult{std::string name;bool passed{false};std::string message;};class TestingCenter{public:void add(const std::string&name,std::function<bool()>fn){tests_.push_back({name,std::move(fn)});}std::vector<TestResult>run()const;private:struct T{std::string n;std::function<bool()>f;};std::vector<T>tests_;};}
