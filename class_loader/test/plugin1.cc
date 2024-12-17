#include <iostream>

#include <cmw/class_loader/class_loader.h>
#include <cmw/class_loader/test/base.h>


class Circle : public Base {
 public:
  virtual void DoSomething() { std::cout << "I am Circle" << std::endl; }
};

class Rect : public Base {
 public:
  virtual void DoSomething() { std::cout << "I am Rect" << std::endl; }
  ~Rect() {}
};

class Triangle : public Base {
 public:
  virtual void DoSomething() { std::cout << "I am Triangle" << std::endl; }
};

class Star : public Base {
 public:
  virtual void DoSomething() { std::cout << "I am Star" << std::endl; }
};

CLASS_LOADER_REGISTER_CLASS(Circle, Base);
CLASS_LOADER_REGISTER_CLASS(Rect, Base);
CLASS_LOADER_REGISTER_CLASS(Triangle, Base);
CLASS_LOADER_REGISTER_CLASS(Star, Base);