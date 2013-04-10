#include "Model.h"
#include "Component.h"
#include "Output.h"

namespace SmartGridToolbox 
{
   Model::~Model() 
   {
   }

   void Model::AddComponent(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = 
         compMap_.insert(make_pair(comp.GetName(), &comp));
      if (result.second == 0) {
         Error("Component already exists in model!");
      }
      else
      {
         comp.Initialize(startTime_);
         compVec_.push_back(&comp);
      }
   }
}
