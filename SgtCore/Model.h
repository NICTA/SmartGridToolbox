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
         typedef std::map<std::string, std::unique_ptr<Component>> ComponentMap;
         typedef std::vector<Component*> ComponentVec;
         typedef std::vector<const Component*> ConstComponentVec;

      public:

         template<typename T, typename... Args> T& newComponent(Args&&... args)
         {
            std::unique_ptr<T> comp(new T(std::forward<Args>(args)...));
            T& t = *comp;
            addOrReplaceGenericComponent(std::move(comp), false);
            return t;
         }

         template<typename T> T& acquireComponent(std::unique_ptr<T>&& comp)
         {
            addOrReplaceGenericComponent(std::move(comp), false);
            return *comp;
         }

         template<typename T, typename... Args> T& replaceComponentWithNew(Args&&... args)
         {
            std::unique_ptr<T> comp(new T(std::forward<Args>(args)...));
            T& t = *comp;
            addOrReplaceGenericComponent(std::move(comp), true);
            return t;
         }

         template<typename T> T& replaceComponent(std::unique_ptr<T>&& comp)
         {
            addOrReplaceGenericComponent(std::move(comp), true);
            return *comp;
         }

         template<typename T> const T* component(const std::string& id) const
         {
            ComponentMap::const_iterator it = compMap_.find(id);
            return (it == compMap_.end()) ? 0 : dynamic_cast<const T*>(it->second.get());
         }
         template<typename T> T* component(const std::string& id)
         {
            return const_cast<T*>((const_cast<const Model*>(this))-> component<T>(id));
         }

         ConstComponentVec components() const;
         ComponentVec components() {return compVec_;}

      private:
         void addOrReplaceGenericComponent(std::unique_ptr<Component>&& comp, bool allowReplace);

      private:
         ComponentMap compMap_;
         ComponentVec compVec_; // Secondary vector encoding order of evaluation/rank.
   };
}

#endif // MODEL_DOT_H
