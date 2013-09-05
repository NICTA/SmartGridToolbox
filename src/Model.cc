#include "Model.h"
#include "Component.h"
#include "WeakOrder.h"
#include <string>

namespace SmartGridToolbox 
{
   Model::~Model() 
   {
   }

   void Model::validate()
   {
      message() << "Model before validation:" << std::endl;
      for (const Component * comp : compVec_)
      {
         message() << "\t" << comp->name() << " " << comp->rank() << std::endl;
         for (Component * const dep : comp->dependencies())
         {
            message() << "\t\t" << dep->name() << " " << dep->rank() << std::endl;
         }
      }

      for (int i = 0; i < compVec_.size(); ++i)
      {
         compVec_[i]->setRank(i);
      }

      WoGraph g(compVec_.size());
      for (int i = 0; i < compVec_.size(); ++i)
      {
         for (const Component * dep : compVec_[i]->dependencies())
         {
            g.link(dep->rank(), i);
         }
      }
      g.weakOrder();
      for (int i = 0; i < g.size(); ++i)
      {
         int idx_i = g.nodes()[i]->index();
         compVec_[idx_i]->setRank(i);
      }
      std::sort(compVec_.begin(), compVec_.end(), 
            [](const Component * lhs, const Component * rhs) -> bool 
            {
               return lhs->rank() < rhs->rank();
            });
      isValid_ = true;
      message() << "Model after validation:" << std::endl;
      for (const Component * comp : compVec_)
      {
         message() << "\t" << comp->name() << " " << comp->rank() << std::endl;
         for (Component * const dep : comp->dependencies())
         {
            message() << "\t\t" << dep->name() << " " << dep->rank() << std::endl;
         }
      }
   }

   void Model::addGenericComponent(Component * comp)
   {
      std::pair<ComponentMap::iterator, bool> result = compMap_.insert(make_pair(comp->name(), comp));
      if (result.second == 0) {
         error() << "Component " << comp->name() << " occurs more than once in the model!" << std::endl;
         abort();
      }
      else
      {
         compVec_.push_back(comp);
         message() << "Component " << comp->name() << " added to model." << std::endl;
      }
   }

}
