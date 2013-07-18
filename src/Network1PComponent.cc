#include "Network1PComponent.h"
#include "Model.h"
#include <iostream>

namespace SmartGridToolbox
{
   void Network1PComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      const std::string nameStr = nd["name"].as<std::string>();
      Network1PComponent * comp = new Network1PComponent;
      comp->setName(nameStr);
      mod.addComponent(*comp);
   }
}
