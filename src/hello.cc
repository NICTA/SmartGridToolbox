#include "Model.h"
#include "Component.h"
#include <string>
using namespace SmartGridToolbox;
using namespace std;

class TestCompA : public Component
{
   public:
      TestCompA(string name) : Component(name) {}
      virtual void UpdateState(TimestampType toTimestep)
      {
         // Empty.
      }
};

class TestCompB : public Component
{
   public:
      TestCompB(string name) : Component(name) {}
      virtual void UpdateState(TimestampType toTimestep)
      {
         // Empty.
      }
      void SetTestCompA(TestCompA & tcA)
      {
         tcA_ = &tcA;
      }
   private:
      TestCompA * tcA_;
};

int main()
{
   Model mod;
   mod.AddComponent(*(new TestCompA("tca1")));
   mod.AddComponent(*(new TestCompB("tcb1")));
   string nm1 = "tca1";
   TestCompA & tca1 = *mod.GetComponent<TestCompA>(nm1);
   //TestCompB & tcb1 = mod.GetComponent("tcb1");
   //tcb1.SetTestCompA(tca1);
}
