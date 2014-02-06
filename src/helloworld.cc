#include <iostream>
#include <vector>

class A
{
   public:
      A() {std::cout << a << std::endl;}
      int a;
};

class B : public A
{
   public:
      B() : a(1) {} 
};

int main()
{
   B b;
}
