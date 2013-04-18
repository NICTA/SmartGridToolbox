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

   void Model::AddPrototype(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = protoMap_.insert(
            make_pair(comp.Name(), &comp));
      if (result.second == 0) {
         Error("Prototype %s already exists in model!", comp.Name().c_str());
      }
      else
      {
         comp.Initialize(not_a_date_time);
         protoVec_.push_back(&comp);
         Message("Prototype %s added to model.", comp.Name().c_str());
      }
   }

   void Model::AddComponent(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = compMap_.insert(
            make_pair(comp.Name(), &comp));
      if (result.second == 0) {
         Error("Component %s already exists in model!", comp.Name().c_str());
      }
      else
      {
         comp.Initialize(not_a_date_time);
         compVec_.push_back(&comp);
         Message("Component %s added to model.", comp.Name().c_str());
      }
   }

   void Model::Validate()
   {
      for (int i = 0; i < compVec_.size(); ++i)
      {
         compVec_[i]->SetRank(i);
      }

      WOGraph g(compVec_.size());
      for (int i = 0; i < compVec_.size(); ++i)
      {
         for (const Component * dep : compVec_[i]->Dependents())
         {
            g.Link(i, dep->Rank());
         }
      }
      g.WeakOrder();
      for (int i = 0; i < g.Size(); ++i)
      {
         compVec_[g[i].Index()]->SetRank(i);
      }
      std::sort(compVec_.begin(), compVec_.end(), [](const Component * lhs,
               const Component * rhs) -> bool {lhs->Rank() < rhs->Rank();});
      isValid_ = true;
   }
}
