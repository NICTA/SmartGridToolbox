#include "Model.h"
#include "Component.h"
#include "Output.h"
#include "WeakOrder.h"
#include <string>

namespace SmartGridToolbox 
{
   Model::~Model() 
   {
   }

   void Model::addPrototype(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = protoMap_.insert(
            make_pair(comp.getName(), &comp));
      if (result.second == 0) {
         error("Prototype %s already exists in model!", comp.getName().c_str());
      }
      else
      {
         comp.initialize(not_a_date_time);
         protoVec_.push_back(&comp);
         message("Prototype %s added to model.", comp.getName().c_str());
      }
   }

   void Model::addComponent(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = compMap_.insert(
            make_pair(comp.getName(), &comp));
      if (result.second == 0) {
         error("Component %s already exists in model!", comp.getName().c_str());
      }
      else
      {
         comp.initialize(not_a_date_time);
         compVec_.push_back(&comp);
         message("Component %s added to model.", comp.getName().c_str());
      }
   }

   void Model::validate()
   {
      message("Model before validation:");
      for (const Component * comp : compVec_)
      {
         message("   %s", comp->getName().c_str());
         for (Component * const dep : comp->getDependencies())
         {
            message("      %s", dep->getName().c_str());
         }
      }
      for (int i = 0; i < compVec_.size(); ++i)
      {
         compVec_[i]->setRank(i);
      }

      WoGraph g(compVec_.size());
      for (int i = 0; i < compVec_.size(); ++i)
      {
         for (const Component * dep : compVec_[i]->getDependencies())
         {
            g.link(dep->getRank(), i);
         }
      }
      g.weakOrder();
      for (int i = 0; i < g.size(); ++i)
      {
         int idx_i = g.getNodes()[i]->getIndex();
         compVec_[idx_i]->setRank(i);
      }
      std::sort(compVec_.begin(), compVec_.end(), 
            [](const Component * lhs, const Component * rhs) -> bool 
            {
               return lhs->getRank() < rhs->getRank();
            });
      isValid_ = true;
      message("Model after validation:");
      for (const Component * comp : compVec_)
      {
         message("   %s %d", comp->getName().c_str(), comp->getRank());
      }
   }
}
