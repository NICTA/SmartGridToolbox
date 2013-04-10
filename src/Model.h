#ifndef MODEL_DOT_H
#define MODEL_DOT_H

#include <map>
#include <vector>
#include <string>

using std::string;

namespace SmartGridToolbox
{
   class Component;

   /// Simulation environment, containing global simulation data.
   /// Units: All units are internally stored in SI.
   class Model
   {
      public:
         /// Default constructor.
         Model()
         {
            // Empty.
         }

         /// Destructor.
         ~Model();

         void AddComponent(Component & comp);

         template<typename T> const T * 
            GetComponentNamed(const std::string & name) const
         {
            ComponentMap::const_iterator it = compMap_.find(name);
            return (it == compMap_.end()) 
               ? 0 : dynamic_cast<const T *>(it->second);
         }

         template<typename T> T * GetComponentNamed(const std::string & name)
         {
            return const_cast<T *>((const_cast<const Model *>(this))->
                  GetComponentNamed<T>(name));
         }

         int NComponents()
         {
            return compVec_.size();
         }

         template<typename T> const T * GetComponentNum(int i) const
         {
            return (i >= 0 && i < compVec_.size()) 
               ? dynamic_cast<T *>(compVec_[i]) : 0;
         }
         template<typename T> T * GetComponentNum(int i)
         {
            return const_cast<T *>((const_cast<const Model *>(this))->
                  GetComponentNum<T>(i));
         }

      private:
         typedef std::map<std::string, Component *> ComponentMap;
         typedef std::vector<Component *> ComponentVec;

         ComponentMap compMap_;
         ComponentVec compVec_;
         ComponentMap prototypeMap_;
         ComponentVec prototypeVec_;
   };
}

#endif // MODEL_DOT_H
