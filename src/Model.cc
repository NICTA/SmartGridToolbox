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
         error() << "Prototype " << comp.getName() << " already exists in model!" << std::endl;
         abort();
      }
      else
      {
         protoVec_.push_back(&comp);
         message() << "Prototype " << comp.getName() << " added to model." << std::endl;
      }
   }

   void Model::validate()
   {
      message() << "Model before validation:" << std::endl;
      for (const Component * comp : compVec_)
      {
         message() << "\t" << comp->getName() << std::endl;
         for (Component * const dep : comp->getDependencies())
         {
            message() << "\t\t" << dep->getName() << std::endl;
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
      message() << "Model after validation:" << std::endl;
      for (const Component * comp : compVec_)
      {
         message() << "\t" << comp->getName() << std::endl;
         for (Component * const dep : comp->getDependencies())
         {
            message() << "\t\t" << dep->getName() << std::endl;
         }
      }
   }

   void Model::addGenericComponent(Component * comp)
   {
      std::pair<ComponentMap::iterator, bool> result = compMap_.insert(make_pair(comp->getName(), comp));
      if (result.second == 0) {
         error() << "Component " << comp->getName() << " occurs more than once in the model!" << std::endl;
         abort();
      }
      else
      {
         compVec_.push_back(comp);
         message() << "Component " << comp->getName() << " added to model." << std::endl;
      }
   }

}
