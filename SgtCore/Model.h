#ifndef MODEL_DOT_H
#define MODEL_DOT_H

#include <SgtCore/Component.h>

#include <map>
#include <vector>
#include <string>

using std::string;

namespace SmartGridToolbox
{
   /// @brief All components and other objects that form the static model that is the subject of the simulation.
   /// @ingroup Core
   class Model
   {
      private:
         typedef std::map<std::string, std::shared_ptr<ComponentAbc>> CompMap;
         typedef std::vector<std::shared_ptr<ComponentAbc>> CompVec;
         typedef std::vector<std::shared_ptr<const ComponentAbc>> ConstCompVec;

      public:

         /// @brief Factory method for ComponentAbcs.
         template<typename T, typename... Args> T& newComponentAbc(Args&&... args)
         {
            auto comp = std::make_shared<T>(std::forward<Args>(args)...);
            return acquireComponentAbc(comp);
         }

         /// @brief Acquire an existing ComponentAbc.
         template<typename T> T& acquireComponentAbc(std::shared_ptr<T> comp)
         {
            addOrReplaceGenericComponentAbc(comp, false);
            return *comp;
         }

         /// @brief Replace an existing ComponentAbc factory method.
         template<typename T, typename... Args> T& replaceComponentAbcWithNew(Args&&... args)
         {
            auto comp = std::make_shared<T>(std::forward<Args>(args)...);
            return replaceComponentAbc(comp);
         }

         /// @brief Replace an existing ComponentAbc with an existing ComponentAbc.
         template<typename T> T& replaceComponentAbc(std::shared_ptr<T> comp)
         {
            addOrReplaceGenericComponentAbc(comp, true);
            return *comp;
         }

         /// @brief Retrieve a const ComponentAbc.
         template<typename T> std::shared_ptr<T> component(const std::string& id) const
         {
            CompMap::const_iterator it = compMap_.find(id);
            return (it == compMap_.end()) ? nullptr : std::dynamic_pointer_cast<const T>(it->second);
         }

         /// @brief Retrieve a ComponentAbc.
         template<typename T> T* component(const std::string& id)
         {
            return const_cast<T*>((const_cast<const Model*>(this))->component<T>(id));
         }

         /// @brief Copied vector of all const ComponentAbcs.
         ConstCompVec components() const;

         /// @brief Copied vector of all ComponentAbcs.
         CompVec components() {return compVec_;}

      private:
         void addOrReplaceGenericComponent(std::unique_ptr<ComponentAbc>&& comp, bool allowReplace);

      private:
         CompMap compMap_;
         CompVec compVec_; // Secondary vector encoding order of evaluation/rank.
   };
}

#endif // MODEL_DOT_H
