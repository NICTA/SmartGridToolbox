#include "Model.h"

#include "Common.h"

#include <algorithm>

namespace SmartGridToolbox
{
   Model::ConstComponentVec Model::components() const
   {
      ConstComponentVec result(compVec_.size());
      std::copy(compVec_.begin(), compVec_.end(), result.begin());
      return result;
   }

   void Model::addOrReplaceGenericComponent(std::unique_ptr<Component> && comp, bool allowReplace)
   {
      Component& ref = *comp;

      message() << "Adding component " << ref.id() << " of type " 
         << ref.componentType() << " to model." << std::endl;
      IndentingOStreamBuf _(messageStream());

      ComponentMap::iterator it1 = compMap_.find(ref.id());
      if (it1 != compMap_.end())
      {
         if (allowReplace)
         {
            it1->second = std::move(comp);
            message() << "Component " << ref.id() << " replaced in model." << std::endl;
         }
         else
         {
            error() << "Component " << ref.id() << " occurs more than once in the model!" << std::endl;
            abort();
         }
      }
      else
      {
         compVec_.push_back(comp.get());
         compMap_[ref.id()] = std::move(comp);
      }
   }
}
