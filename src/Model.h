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
         Model() : name_("null")
         {
            // Empty.
         }

         /// Destructor.
         ~Model();

         const std::string & Name() const
         {
            return name_;
         }
         const void SetName(const std::string & name)
         {
            name_ = name;
         }

         void AddPrototype(Component & comp);

         template<typename T> const T * 
            PrototypeNamed(const std::string & name) const
         {
            ComponentMap::const_iterator it = protoMap_.find(name);
            return (it == protoMap_.end()) 
               ? 0 : dynamic_cast<const T *>(it->second);
         }
         template<typename T> T * PrototypeNamed(const std::string & name)
         {
            return const_cast<T *>((const_cast<const Model *>(this))->
                  PrototypeNamed<T>(name));
         }

         int NPrototypes()
         {
            return protoMap_.size();
         }

         ComponentVec::const_iterator beginPrototype() const
         {
            return protoVec_.begin();
         }
         ComponentVec::iterator beginPrototype()
         {
            return protoVec_.begin();
         }

         ComponentVec::const_iterator endPrototype() const
         {
            return protoVec_.end();
         }
         ComponentVec::iterator endPrototype()
         {
            return protoVec_.end();
         }

         void AddComponent(Component & comp);

         template<typename T> const T * 
            ComponentNamed(const std::string & name) const
         {
            ComponentMap::const_iterator it = compMap_.find(name);
            return (it == compMap_.end()) 
               ? 0 : dynamic_cast<const T *>(it->second);
         }
         template<typename T> T * ComponentNamed(const std::string & name)
         {
            return const_cast<T *>((const_cast<const Model *>(this))->
                  ComponentNamed<T>(name));
         }

         int NComponents()
         {
            return compVec_.size();
         }

         template<typename T> const T * ComponentNum(int i) const
         {
            return (i >= 0 && i < compVec_.size()) 
               ? dynamic_cast<T *>(compVec_[i]) : 0;
         }
         template<typename T> T * ComponentNum(int i)
         {
            return const_cast<T *>((const_cast<const Model *>(this))->
                  ComponentNum<T>(i));
         }

         ComponentVec::const_iterator beginComponent() const
         {
            return compVec_.begin();
         }
         ComponentVec::iterator beginComponent()
         {
            return compVec_.begin();
         }

         ComponentVec::const_iterator endComponent() const
         {
            return compVec_.end();
         }
         ComponentVec::iterator endComponent()
         {
            return compVec_.end();
         }

      private:
         void OrderComponents(); 
            // Produce weak ordering based on reachability.

      private:
         std::string name_;
         ComponentMap protoMap_;
         ComponentVec protoVec_;
         ComponentMap compMap_;
         ComponentVec compVec_;
   };
}

#endif // MODEL_DOT_H
