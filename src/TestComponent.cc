#include "TestComponent.h"
#include "Model.h"
 
namespace SmartGridToolbox
{
   void TestComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      const std::string name = nd["name"].as<std::string>();
      message() << "TestComponentParser: name = " << name << std::endl;
      const int value = nd["value"].as<int>();
      message() << "TestComponentParser: value = " << value << std::endl;
      TestComponent & tc = mod.newComponent<TestComponent>(name);
      tc.setName(name);
      tc.setValue(value);
   }

   void TestComponentParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      const std::string myName = nd["name"].as<std::string>();
      const std::string anotherName = nd["another"].as<std::string>();
      TestComponent * comp = mod.getComponentNamed<TestComponent>(myName);
      TestComponent * another = mod.getComponentNamed<TestComponent>(anotherName);
      comp->setAnother(another);
   }
}
