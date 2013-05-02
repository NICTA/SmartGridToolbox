#include "Common.h"
#include "Component.h"
#include "Model.h"
#include "Output.h"
#include "Parser.h"
#include "Random.h"
#include "Simulation.h"
#include <string>
#include <iostream>

using namespace SmartGridToolbox;
using namespace std;

class TestCompA : public Component
{
   public:
      TestCompA(string name) : Component(name) {}
      virtual void advanceToTime(ptime t)
      {
         std::cout << "TCA AdvanceToTimestep " << t << std::endl;
      }
   private:
};

class TestCompB : public Component
{
   public:
      TestCompB(string name) : Component(name) {}
      virtual void advanceToTime(ptime t)
      {
         tcA_->advanceToTime(t);
      }
      void setTestCompA(TestCompA & tcA)
      {
         tcA_ = &tcA;
         dependsOn(tcA);
      }
   private:
      TestCompA * tcA_;
};

int main()
{
   Model mod;
   Simulation sim(mod);
   Parser parser;
   parser.parse("sample_config.yaml", mod, sim);
   mod.addComponent(*(new TestCompA("tca1")));
   mod.addComponent(*(new TestCompB("tcb1")));
   TestCompA & tca1 = *mod.getComponentNamed<TestCompA>("tca1");
   TestCompB & tcb1 = *mod.getComponentNamed<TestCompB>("tcb1");
   tcb1.setTestCompA(tca1);
   mod.validate();
   date d(2012, Jan, 10);
   ptime p(d, hours(1));
   tcb1.advanceToTime(p);

   for (int i = 0; i < 10; ++i)
   {
      double d = randNormal(10.0, 3.0);
      std::cout << "Random = " << d << std::endl;
   }

   double probs[] = {0.2, 0.3, 0.5};
   for (int i = 0; i < 20; ++i)
   {
      int d = randDiscrete(probs, 3);
      std::cout << "Random = " << d << std::endl;
   }

   Complex c(1.0, 2.0);
   std::cout << "Complex = " << c << std::endl;
   std::cout << "Complex real = " << c.real() << std::endl;
}
