#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/Component.h>
#include "WeakOrder.h"
#include <algorithm>
#include <string>

namespace SmartGridToolbox 
{
   Model::ConstComponentVec Model::components() const
   {
      ConstComponentVec result(compVec_.size());
      std::copy(compVec_.begin(), compVec_.end(), result.begin());
      return result;
   }

   void Model::validate()
   {
      SGT_DEBUG
      (
         debug() << "Model before validation:" << std::endl;
         for (const Component* comp : compVec_)
         {
            debug() << "\t" << comp->name() << " " << comp->rank() << std::endl;
            for (const Component* const dep : comp->dependencies())
            {
               debug() << "\t\t" << dep->name() << " " << dep->rank() << std::endl;
            }
         }
      )

      for (int i = 0; i < compVec_.size(); ++i)
      {
         compVec_[i]->setRank(i);
      }

      WoGraph g(compVec_.size());
      for (int i = 0; i < compVec_.size(); ++i)
      {
         // At this point, rank of component i is i.
         for (const Component* dep : compVec_[i]->dependencies())
         {
            // i depends on dep->rank().
            g.link(dep->rank(), i);
         }
      }
      g.weakOrder();
      int newRank = 0;
      for (int i = 0; i < g.size(); ++i)
      {
         if (i > 0 && g.nodes()[i-1]->dominates(*g.nodes()[i]))
         {
            ++newRank;
         }
         int idx_i = g.nodes()[i]->index();
         compVec_[idx_i]->setRank(newRank);
      }
      std::sort(compVec_.begin(), compVec_.end(), 
            [](const Component* lhs, const Component* rhs) -> bool 
            {
               return ((lhs->rank() < rhs->rank()) ||
                       ((lhs->rank() == rhs->rank()) && (lhs->name() < rhs->name())));
            });
      isValid_ = true;
      message() << "Model after validation:" << std::endl;
      for (const Component* comp : compVec_)
      {
         message() << "\t" << comp->name() << " " << comp->rank() << std::endl;
         for (const Component* const dep : comp->dependencies())
         {
            message() << "\t\t" << dep->name() << " " << dep->rank() << std::endl;
         }
      }
   }

   void Model::addOrReplaceGenericComponent(std::unique_ptr<Component> && comp, bool allowReplace)
   {
      Component & ref = *comp;
      ComponentMap::iterator it1 = compMap_.find(ref.name());
      if (it1 != compMap_.end())
      {
         if (allowReplace)
         {
            it1->second = std::move(comp);
            message() << "Component " << ref.name() << " replaced in model." << std::endl;
         }
         else
         {
            error() << "Component " << ref.name() << " occurs more than once in the model!" << std::endl;
            abort();
         }
      }
      else
      {
         compVec_.push_back(comp.get());
         compMap_[ref.name()] = std::move(comp);
         message() << "Component " << ref.name() << " added to model." << std::endl;
      }
   }
}
