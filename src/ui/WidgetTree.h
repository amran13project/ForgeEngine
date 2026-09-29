#pragma once
#include <string>
#include <vector>
#include <memory>
namespace forge::ui {
struct Rect { float x{},y{},w{},h{}; };
class Widget {
public:
    explicit Widget(std::string type):type_(std::move(type)){}
    void setName(std::string n){name_=std::move(n);} const std::string& name()const{return name_;}
    void setRect(Rect r){rect_=r;} Rect rect()const{return rect_;}
    void add(std::unique_ptr<Widget> child){children_.push_back(std::move(child));}
    const std::vector<std::unique_ptr<Widget>>& children()const{return children_;}
    const std::string& type()const{return type_;}
private:
    std::string type_,name_; Rect rect_{}; std::vector<std::unique_ptr<Widget>> children_;
};
class UIContext { public: std::unique_ptr<Widget> create(const std::string&type)const{return std::make_unique<Widget>(type);} };
}
