#ifndef MODEL_DOT_H
#define MODEL_DOT_H

#include <map>
#include <vector>
#include <string>
#include "Common.h"

using std::string;

namespace SmartGridToolbox
{
   class Component;

   /// Simulation environment, containing global simulation data.
   /// Units: All units are internally stored in SI.
   class Model
   {
      public:
         typedef std::map<std::string, Component *> ComponentMap;
         typedef std::vector<Component *> ComponentVec;

      public:
         /// Default constructor.
         Model() : name_("null"),
                   isValid_(false)
         {
            // Empty.
         }

         /// Destructor.
         ~Model();

         const std::string & getName() const
         {
            return name_;
         }
         const void setName(const std::string & name)
         {
            name_ = name;
         }

         void addPrototype(Component & comp);

         template<typename T> const T * getPrototypeNamed(const std::string & name) const
         {
            ComponentMap::const_iterator it = protoMap_.find(name);
            return (it == protoMap_.end()) ? 0 : dynamic_cast<const T *>(it->second);
         }
         template<typename T> T * getPrototypeNamed(const std::string & name)
         {
            return const_cast<T *>((const_cast<const Model *>(this))->getPrototypeNamed<T>(name));
         }

         int nPrototypes()
         {
            return protoMap_.size();
         }

         const ComponentVec & getPrototypes() const
         {
            return protoVec_;
         }

         template<typename T, typename... Args> T & addComponent(Args&&... args)
         {
            T * comp = new T(std::forward<Args>(args)...);
            addGenericComponent(comp);
            return *comp;
         }

         template<typename T> const T * getComponentNamed(const std::string & name) const
         {
            ComponentMap::const_iterator it = compMap_.find(name);
            return (it == compMap_.end()) ? 0 : dynamic_cast<const T *>(it->second);
         }
         template<typename T> T * getComponentNamed(const std::string & name)
         {
            return const_cast<T *>((const_cast<const Model *>(this))-> getComponentNamed<T>(name));
         }

         int NComponents()
         {
            return compVec_.size();
         }

         template<typename T> const T * getComponentAt(int i) const
         {
            return (i >= 0 && i < compVec_.size()) ? dynamic_cast<T *>(compVec_[i]) : 0;
         }
         template<typename T> T * getComponentAt(int i)
         {
            return const_cast<T *>((const_cast<const Model *>(this))-> getComponentAt<T>(i));
         }

         const ComponentVec & getComponents() const
         {
            return compVec_;
         }
         ComponentVec & getComponents()
         {
            return compVec_;
         }

         void validate();

      private:
         void addGenericComponent(Component * comp);

      private:
         std::string name_;
         ComponentMap protoMap_;
         ComponentVec protoVec_;
         ComponentMap compMap_;
         ComponentVec compVec_;
         bool isValid_;
   };
}

#endif // MODEL_DOT_H
