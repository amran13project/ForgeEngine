#include "testing/TestingCenter.h"
namespace forge::testing {std::vector<TestResult> TestingCenter::run()const{std::vector<TestResult>r;for(const auto&t:tests_){try{bool ok=t.f();r.push_back({t.n,ok,ok?"PASS":"FAIL"});}catch(const std::exception&e){r.push_back({t.n,false,e.what()});}}return r;}}
