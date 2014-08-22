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

      public:

         /// @brief Factory method for Components.
         template<typename T, typename... Args> T& newComponent(Args&&... args)
         {
            auto comp = std::make_shared<T>(std::forward<Args>(args)...);
            return acquireComponent(comp);
         }

         /// @brief Acquire an existing Component.
         template<typename T> T& acquireComponent(std::shared_ptr<T> comp)
         {
            addOrReplaceGenericComponent(comp, false);
            return *comp;
         }

         /// @brief Replace an existing Component factory method.
         template<typename T, typename... Args> T& replaceComponentWithNew(Args&&... args)
         {
            auto comp = std::make_shared<T>(std::forward<Args>(args)...);
            return replaceComponent(comp);
         }

         /// @brief Replace an existing Component with an existing Component.
         template<typename T> T& replaceComponent(std::shared_ptr<T> comp)
         {
            addOrReplaceGenericComponent(comp, true);
            return *comp;
         }

         /// @brief Retrieve a const Component.
         template<typename T> std::shared_ptr<T> component(const std::string& id) const
         {
            CompMap::const_iterator it = compMap_.find(id);
            return (it == compMap_.end()) ? nullptr : std::dynamic_pointer_cast<const T>(it->second);
         }

         /// @brief Retrieve a Component.
         template<typename T> T* component(const std::string& id)
         {
            return const_cast<T*>((const_cast<const Model*>(this))->component<T>(id));
         }

         /// @brief Access the components.
         const CompMap& components()
         {
            return compMap_;
         }

      private:

         void addOrReplaceGenericComponent(std::shared_ptr<ComponentAbc> comp, bool allowReplace);

      private:

         CompMap compMap_;
   };
}

#endif // MODEL_DOT_H
