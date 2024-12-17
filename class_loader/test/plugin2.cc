#include <iostream>

#include <cmw/class_loader/class_loader.h>
#include <cmw/class_loader/test/base.h>

class Apple : public Base {
 public:
  virtual void DoSomething() { std::cout << "I am Apple" << std::endl; }
};

class Pear : public Base {
 public:
  virtual void DoSomething() { std::cout << "I am Pear!!!" << std::endl; }
};

class Banana : public Base {
 public:
  virtual void DoSomething() { std::cout << "I am Banana" << std::endl; }
};

class Peach : public Base {
 public:
  virtual void DoSomething() { std::cout << "I am Peach!!!" << std::endl; }
};

CLASS_LOADER_REGISTER_CLASS(Apple, Base);
CLASS_LOADER_REGISTER_CLASS(Pear, Base);
CLASS_LOADER_REGISTER_CLASS(Banana, Base);
CLASS_LOADER_REGISTER_CLASS(Peach, Base);
