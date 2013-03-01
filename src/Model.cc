#include "Model.h"
#include "Object.h"

namespace MGSim 
{
   Model::~Model() 
   {
   }

   void Model::AddObject(Object * obj)
   {
      std::pair<ObjectMapType::iterator, bool> result = 
         objectMap_.insert(make_pair(obj->GetName(), obj));
      if (result.second == 0) {
         throw "ERROR: Object already exists in model!";
      }
   }
}
