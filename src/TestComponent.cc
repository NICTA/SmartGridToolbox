#include "TestComponent.h"
#include "Model.h"
 
namespace SmartGridToolbox
{
   void TestComponentParser::parse(const YAML::Node & comp, Model & mod) const
   {
      const std::string name = comp["name"].as<std::string>();
      message("TestComponentParser: name = %s", name.c_str());
      const int value = comp["value"].as<int>();
      message("TestComponentParser: value = %d", value);
      TestComponent * tc = new TestComponent;
      tc->setName(name);
      tc->setValue(value);
      mod.addComponent(*tc);
   }
}
