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
            tInit_(not_a_date_time),
            t_(not_a_date_time),
            rank_(-1)
         {
            // Empty.
         }

         /// Constructor.
         Component() : Component("unknown")
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
         const std::string & getName() const
         {
            return name_;
         }

         /// Set the name of the object.
         void setName(const std::string & name)
         {
            name_ = name;
         }
         
         /// Get the current step for the object.
         ptime getTime() const
         {
            return t_;
         }

         /// Get the current step for the object.
         ptime getInitTime() const
         {
            return tInit_;
         }

         /** @name Rank
          *  Rank: A < B means B depends on A, not vice-versa, so A should go first. */
         ///@{

         /// Get the rank of the object.
         int getRank() const
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

         const std::vector<Component *> & getDependencies() const
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

         /// Reset state of the object, time to timestamp t.
         /** @param t */
         void initialize(const ptime t = not_a_date_time)
         {
            t_ = t;
            tInit_ = t;
            initializeState(t);
         }

         /// Bring state up to time t if necessary, throwing error if beyond t.
         /** @param t the timestamp to advance to. */
         void ensureAtTime(ptime t);

         /// Bring state up to time t.
         /** @param t the timestamp to advance to. */
         void update(ptime t)
         {
            willUpdate_.trigger();
            updateState(t_, t);
            t_ = t;
            didUpdate_.trigger();
         }

         virtual ptime getValidUntil() const
         {
            return pos_infin;
         }

         /// @name Events
         /// @{
         Event & getEventWillUpdate() {return willUpdate_;}
         Event & getEventDidUpdate() {return didUpdate_;}
         /// @}

         /// @}

         /// @name Properties
         /// @{

         template <typename T, PropType A> const Property<T, A> * getProperty(const std::string & name) const
         {
            PropertyMap::const_iterator it = propertyMap_.find(name);
            return (it == propertyMap_.end()) ? 0 : dynamic_cast<const Property<T, A> *>(it->second);
         }

         template <typename T, PropType A> Property<T, A> * getProperty(const std::string & name)
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
         /// Reset state of the object, time is at timestamp t_.
         virtual void initializeState(ptime t)
         {
            // Empty.
         }

         /// Bring state up to time t_.
         virtual void updateState(ptime t0, ptime t1)
         {
            // Empty.
         }
         
      private:
         std::string name_;
         ptime tInit_; ///< The initial time.
         ptime t_; ///< The current time.
         ComponentVec dependencies_; ///< I depend on these.
         int rank_;  ///< Evaluation rank, based on weak ordering.
         PropertyMap propertyMap_;
         Event willUpdate_;
         Event didUpdate_;
   };
}

#endif // COMPONENT_DOT_H
