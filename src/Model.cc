#include "Model.h"
#include "Component.h"
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
         SGTError("Prototype " << comp.getName() << " already exists in model!");
      }
      else
      {
         protoVec_.push_back(&comp);
         message("Prototype " << comp.getName() << " added to model.");
      }
   }

   void Model::addComponent(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = compMap_.insert(
            make_pair(comp.getName(), &comp));
      if (result.second == 0) {
         SGTError("Component " << comp.getName() << " already exists in model!");
      }
      else
      {
         compVec_.push_back(&comp);
         message("Component " << comp.getName() << " added to model.");
      }
   }

   void Model::validate()
   {
      message("Model before validation:");
      for (const Component * comp : compVec_)
      {
         message("   " << comp->getName());
         for (Component * const dep : comp->getDependencies())
         {
            message("      " << dep->getName());
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
         message("   " << comp->getName());
         for (Component * const dep : comp->getDependencies())
         {
            message("      " << dep->getName());
         }
      }
   }
}
