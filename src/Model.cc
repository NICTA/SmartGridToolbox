#include "Model.h"
#include "Component.h"
#include "Output.h"
#include <string>

namespace SmartGridToolbox 
{
   Model::~Model() 
   {
   }

   void Model::AddPrototype(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = protoMap_.insert(
            make_pair(comp.GetName(), &comp));
      if (result.second == 0) {
         Error("Prototype %s already exists in model!", comp.GetName().c_str());
      }
      else
      {
         comp.Initialize(not_a_date_time);
         protoVec_.push_back(&comp);
         Message("Prototype %s added to model.", comp.GetName().c_str());
      }
   }

   void Model::AddComponent(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = compMap_.insert(
            make_pair(comp.GetName(), &comp));
      if (result.second == 0) {
         Error("Component %s already exists in model!", comp.GetName().c_str());
      }
      else
      {
         comp.Initialize(not_a_date_time);
         compVec_.push_back(&comp);
         Message("Component %s added to model.", comp.GetName().c_str());
      }
   }
}
