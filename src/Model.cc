#include "Model.h"
#include "Component.h"
#include "Output.h"
#include <string>

namespace SmartGridToolbox 
{
   Model::~Model() 
   {
   }

   void Model::AddComponent(Component & comp, bool isPrototype)
   {
      const char * type;
      ComponentMap * map;
      ComponentVec * vec;
      if (isPrototype)
      {
         type = "Prototype";
         map = &protoMap_;
         vec = &protoVec_;
      }
      else
      {
         type = "Component";
         map = &compMap_;
         vec = &compVec_;
      }

      std::pair<ComponentMap::iterator, bool> result = 
         map->insert(make_pair(comp.GetName(), &comp));
      if (result.second == 0) {
         const char * cstr = comp.GetName().c_str();
         Error("%s %s already exists in model!", type, comp.GetName().c_str());
      }
      else
      {
         comp.Initialize(startTime_);
         vec->push_back(&comp);
         Message("%s %s added to model.", type, comp.GetName().c_str());
      }
   }
}
