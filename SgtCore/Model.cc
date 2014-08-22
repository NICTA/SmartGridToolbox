#include "Model.h"

#include "Common.h"

#include <algorithm>

namespace SmartGridToolbox
{
   void Model::addOrReplaceGenericComponent(std::shared_ptr<ComponentAbc> comp, bool allowReplace)
   {
      message() << "Adding component " << comp->id() << " of type " 
         << comp->componentTypeStr() << " to model." << std::endl;
      IndentingOStreamBuf _(messageStream());

      CompMap::iterator it1 = compMap_.find(comp->id());
      if (it1 != compMap_.end())
      {
         if (allowReplace)
         {
            it1->second = comp;
            message() << "Component " << comp->id() << " replaced in model." << std::endl;
         }
         else
         {
            error() << "Component " << comp->id() << " occurs more than once in the model!" << std::endl;
            abort();
         }
      }
      else
      {
         compMap_[comp->id()] = comp;
      }
   }
}
