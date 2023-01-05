#include "function.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>

double foo(int i, std::string s) {
  std::cout << "double foo(int, string)\n";
  return 0.99;
}

class X {
public:
  double operator()(int i, std::string s) const {
    std::cout << "double X::operator(int, string)\n";
    return 32.3;
  }

  static double bar(int i, std::string s) {
    std::cout << "static double X::bar(int, string)\n";
    return 92.2;
  }
};

auto lamb = [](int i, std::string s) -> double {
  std::cout << "double lambda(int, string)\n";
  return 98.323;
};

int main() {
  xdbd::Function<double(int, std::string)> f; // default ctor
  assert(!f);

  xdbd::Function<double(int, std::string)> f2 = foo; // ctor(T&&)
  f2(3, "hello");

  xdbd::Function<double(int, std::string)> f3 =
      static_cast<xdbd::Function<double(int, std::string)> const>(
          f2); // ctor(const&)
  f3(3, "hello");

  xdbd::Function<double(int, std::string)> f4 = f2; // ctor(&)
  f4(3, "hello");

  xdbd::Function<double(int, std::string)> f5 = std::move(f2); // ctor(&&);
  f5(3, "hello");

  xdbd::Function<double(int, std::string)> const f6 = foo;
  f6(3, "hello");

  f = f6; // =(const&)
  f(3, "hello");

  f = f5; // =(&)
  f(3, "hello");

  f = std::move(f3); // =(&&)
  f(3, "hello");

  f = foo; // =(T&&)
  f(3, "hello");

  f = X(); // =(T&&)
  f(3, "hello");

  xdbd::Function<double(int, std::string)> f7 = lamb;
  f7(3, "helloo");

  f = lamb;
  f(3, "hello");

  f = &X::bar;
  f(3, "hello");

  return 0;
}
