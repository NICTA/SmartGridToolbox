#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <string>
#include <vector>
#include <map>
#include "Common.h"
#include "Event.h"
#include "Property.h"

namespace SmartGridToolbox
{
   /// Base class for simulation objects.
   class Component
   {
      public:
         /// @name Lifecycle
         /// @{
         
         /// Constructor.
         /** @param name the unique name */
         explicit Component(const std::string & name) : 
            name_(name),
            currentTime_(not_a_date_time),
            rank_(-1),
            willUpdate_("Component " + name_ + " will update"),
            didUpdate_("Component " + name_ + " did update"),
            needsUpdate_("Component " + name_ + " needs update")
         {
            // Empty.
         }

         /// Destructor.
         virtual ~Component()
         {
            for (auto it : propertyMap_)
            {
               delete it.second;
            }
         }

         /// @}

         /// Get the name of the object.
         const std::string & name() const
         {
            return name_;
         }

         /// Set the name of the object.
         void setName(const std::string & name)
         {
            name_ = name;
         }
         
         /// Get the current step for the object.
         Time time() const
         {
            return currentTime_;
         }

         /// Get the initial time for the object.
         Time initTime() const
         {
            return startTime_;
         }

         /** @name Rank
          *  Rank: A < B means B depends on A, not vice-versa, so A should go first. Thus, evaluation should
          *  proceed from lowest ranked to highest ranked objects. */
         ///@{

         /// Get the rank of the object.
         int rank() const
         {
            return rank_;
         }

         /// Set the rank of the object.
         void setRank(int rank)
         {
            rank_ = rank;
         }

         /// @}

         /// @name Dependencies.
         ///@{

         const std::vector<Component *> & dependencies() const
         {
            return dependencies_;
         }

         /// Dependents update after I update.
         void dependsOn(Component & b)
         /** @param t Component on which I depend. */
         {
            dependencies_.push_back(&b);
         }

         /// @}

         /// @name Simulation
         /// @{

         /// Reset state of the object, time to t.
         /** @param t */
         void initialize(Time t);

         /// Bring state up to time t.
         /** @param t the Time to advance to. */
         void update(Time t);

         virtual Time validUntil() const
         {
            return pos_infin;
         }

         /// @name Events
         /// @{
         Event & eventWillUpdate() {return willUpdate_;}
         Event & eventDidUpdate() {return didUpdate_;}
         Event & eventNeedsUpdate() {return needsUpdate_;}
         /// @}

         /// @}

         /// @name Properties
         /// @{

         template <typename T, PropType A> const Property<T, A> * property(const std::string & name) const
         {
            PropertyMap::const_iterator it = propertyMap_.find(name);
            return (it == propertyMap_.end()) ? 0 : dynamic_cast<const Property<T, A> *>(it->second);
         }

         template <typename T, PropType A> Property<T, A> * property(const std::string & name)
         {
            PropertyMap::const_iterator it = propertyMap_.find(name);
            return (it == propertyMap_.end()) ? 0 : dynamic_cast<Property<T, A> *>(it->second);
         }

         template<typename T, PropType A, typename U> void addProperty(const std::string & name, U u)
         {
            propertyMap_[name] = new Property<T, A>(u);
         }

         template<typename T, PropType A, typename G, typename S> void addProperty(const std::string & name, G g, S s)
         {
            propertyMap_[name] = new Property<T, A>(g, s);
         }

         /// @}

      protected:
         typedef std::vector<Component *> ComponentVec;
         typedef std::map<const std::string, PropertyBase *> PropertyMap;

      private:
         /// Reset state of the object, time is at currentTime_.
         virtual void initializeState(Time t)
         {
            // Empty.
         }

         /// Bring state up to time currentTime_.
         virtual void updateState(Time t0, Time t1)
         {
            // Empty.
         }
         
      private:
         std::string name_;
         Time currentTime_; ///< The current time.
         Time startTime_; ///< The initial time.
         ComponentVec dependencies_; ///< I depend on these.
         int rank_;  ///< Evaluation rank, based on weak ordering.
         PropertyMap propertyMap_;
         Event willUpdate_;
         Event didUpdate_;
         Event needsUpdate_;
   };
}

#endif // COMPONENT_DOT_H
