#ifndef MODEL_DOT_H
#define MODEL_DOT_H

#include <SgtSim/Component.h>
#include <SgtSim/TimeSeries.h>

#include <SgtCore/Common.h>

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
         typedef std::map<std::string, std::unique_ptr<TimeSeriesBase>> TimeSeriesMap;
         typedef std::vector<Component*> ComponentVec;
         typedef std::vector<const Component*> ConstComponentVec;

      public:
         /// @brief Default constructor.
         Model() : name_("null"),
                   isValid_(false),
                   timezone_(new local_time::posix_time_zone("UTC0"))
         {
            // Empty.
         }

         const std::string& name() const {return name_;}
         const void setName(const std::string& name) {name_ = name;}

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

         template<typename T> const T* component(const std::string& name) const
         {
            ComponentMap::const_iterator it = compMap_.find(name);
            return (it == compMap_.end()) ? 0 : dynamic_cast<const T*>(it->second.get());
         }
         template<typename T> T* component(const std::string& name)
         {
            return const_cast<T*>((const_cast<const Model*>(this))-> component<T>(name));
         }

         ConstComponentVec components() const;
         ComponentVec components() {return compVec_;}

         template<typename T> const T* timeSeries(const std::string& name) const
         {
            TimeSeriesMap::const_iterator it = timeSeriesMap_.find(name);
            return (it == timeSeriesMap_.end()) ? 0 : dynamic_cast<const T*>(it->second.get());
         }
         template<typename T> T* timeSeries(const std::string& name)
         {
            return const_cast<T*>((const_cast<const Model*>(this))-> timeSeries<T>(name));
         }

         void acquireTimeSeries (const std::string& name, std::unique_ptr<TimeSeriesBase> timeSeries)
         {
            timeSeriesMap_[name] = std::move(timeSeries);
         }

         LatLong latLong() const {return latLong_;}
         void setLatLong(const LatLong& latLong) {latLong_ = latLong;}

         const local_time::time_zone_ptr timezone() const {return timezone_;}
         void setTimezone(local_time::time_zone_ptr tz) {timezone_ = tz;}

         void validate();

      private:
         void addOrReplaceGenericComponent(std::unique_ptr<Component>&& comp, bool allowReplace);
         void printDependencies(std::ostream& os);

      private:
         std::string name_;
         bool isValid_;

         ComponentMap compMap_;
         TimeSeriesMap timeSeriesMap_;

         ComponentVec compVec_; // Secondary vector encoding order of evaluation/rank.

         LatLong latLong_;
         local_time::time_zone_ptr timezone_;
   };
}

#endif // MODEL_DOT_H
