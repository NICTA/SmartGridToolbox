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
      for (int i = 0; i < compVec_.size(); ++i)
      {
         compVec_[i]->setRank(i);
      }

      WoGraph g(compVec_.size());
      for (int i = 0; i < compVec_.size(); ++i)
      {
         for (const Component * dep : compVec_[i]->Dependents())
         {
            g.link(i, dep->getRank());
         }
      }
      g.weakOrder();
      for (int i = 0; i < g.Size(); ++i)
      {
         compVec_[g[i].getIndex()]->setRank(i);
      }
      std::sort(compVec_.begin(), compVec_.end(), [](const Component * lhs,
               const Component * rhs) -> bool 
               {lhs->getRank() < rhs->getRank();});
      isValid_ = true;
   }
}
