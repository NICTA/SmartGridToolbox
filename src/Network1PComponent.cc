#include "Network1PComponent.h"

namespace SmartGridToolbox
{
   void Network1PComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      const std::string name = nd["name"].as<std::string>();
      Network1PComponent * comp = new Network1PComponent;
      comp->setName(name);
      mod.addComponent(*comp);
   }
   virtual void Network1PComponent::initializeState(ptime t) override {};
   virtual void updateState(ptime t0, ptime t1) override {};
}
