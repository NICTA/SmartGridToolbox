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
         /// Default constructor.
         Model() : name_("null")
         {
            // Empty.
         }

         /// Destructor.
         ~Model();

         const std::string & GetName() const
         {
            return name_;
         }
         const void SetName(const std::string & name)
         {
            name_ = name;
         }

         const ptime & GetStartTime() const
         {
            return startTime_;
         }
         const void SetStartTime(const ptime & startTime)
         {
            startTime_ = startTime;
         }

         const ptime & GetEndTime() const
         {
            return endTime_;
         }
         const void SetEndTime(const ptime & endTime)
         {
            endTime_ = endTime;
         }

         void AddComponent(Component & comp, bool isPrototype = false);

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

      private:
         std::string name_;
         ptime startTime_;
         ptime endTime_;
         ComponentMap compMap_;
         ComponentVec compVec_;
         ComponentMap protoMap_;
         ComponentVec protoVec_;
   };
}

#endif // MODEL_DOT_H
