#include <iostream>
#include <memory>
#include <vector>

class A {};
class B : public A {};

class Parser
{
   public:
      virtual void parse();
};

class ParserA : public Parser
{
   public:
      virtual void parse()
      {
         a = std::make_shared<A>();
      }
      std::shared_ptr<A> a;
};

class ParserB : public ParserA
{
   public:
      virtual void parse()
      {
         a = std::make_shared<B>();
      }
};

void Parser::parse() {}

int main()
{
   std::vector<std::shared_ptr<Parser>> pVec;
   pVec.push_back(std::make_shared<ParserA>());
   pVec.push_back(std::make_shared<ParserB>());

   for (auto p : pVec)
   {
      p->parse();
   }
}
