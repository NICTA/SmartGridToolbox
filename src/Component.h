#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <string>
#include <vector>
#include <map>
#include "Common.h"
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
         explicit Component(const std::string & name) : name_(name),
                                                        rank_(-1)
         {
            // Empty.
         }

         /// Pure virtual destructor.
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
         
         /// Get the current step for the object.
         ptime getTimestamp()
         {
            return t_;
         }


         /** @name Rank
          *  Rank: A < B means B depends on A, not vice-versa, so A should go
          *  first. */
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

         ///@}

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

         ///@}

         /// @name Simulation
         /// @{

         /// Reset state of the object, time to timestamp t.
         /** @param t */
         void initializeComponent(const ptime t = not_a_date_time)
         {
            t_ = t;
            initialize();
         }

         /// Bring state up to time t.
         /** @param t the timestamp to advance to. */
         void update(ptime t)
         {
            updateState(t);
            t_ = t;
         }

         virtual ptime getValidUntil() const
         {
            return pos_infin;
         }

         /// @}

         /// @name Properties
         /// @{

         template <typename T>
         const ReadProperty<T> * getReadProperty(const std::string & name) const
         {
            PropertyMap::const_iterator it = propertyMap_.find(name);
            return (it == propertyMap_.end()) 
               ? 0 : dynamic_cast<const ReadProperty<T> *>(it->second);
         }

         template <typename T>
         WriteProperty<T> * getWriteProperty(const std::string & name) const
         {
            PropertyMap::const_iterator it = propertyMap_.find(name);
            return (it == propertyMap_.end()) 
               ? 0 : dynamic_cast<WriteProperty<T> *>(it->second);
         }

         template <typename T>
         ReadWriteProperty<T> * getReadWriteProperty(
               const std::string & name) const
         {
            PropertyMap::const_iterator it = propertyMap_.find(name);
            return (it == propertyMap_.end()) 
               ? 0 : dynamic_cast<ReadWriteProperty<T> *>(it->second);
         }

         template<typename T, typename G>
         void addReadProperty(const std::string & name, G get)
         {
            ReadProperty<T> * p = new ReadProperty<T>(get);
            propertyMap_[name] = p;
         }

         template<typename T, typename S>
         void addWriteProperty(const std::string & name, S set)
         {
            WriteProperty<T> * p = new WriteProperty<T>(set);
            propertyMap_[name] = p;
         }

         template<typename T, typename G, typename S>
         void addReadWriteProperty(const std::string & name, G get, S set)
         {
            ReadWriteProperty<T> * p = new ReadWriteProperty<T>(get, set);
            propertyMap_[name] = p;
         }
         /// @}

      protected:
         typedef std::vector<Component *> ComponentVec;
         typedef std::map<const std::string, PropertyBase *> PropertyMap;

      private:
         /// Reset state of the object, time is at timestamp t_.
         virtual void initialize()
         {
            // Empty.
         }

         /// Bring state up to time t_.
         virtual void updateState(ptime t)
         {
            // Empty.
         }
         
      private:
         std::string name_;
         ptime t_; // The current time.
         ComponentVec dependencies_; // I depend on these.
         int rank_;  // Evaluation rank, based on weak ordering.
         PropertyMap propertyMap_;
         PropertyMap controlMap_;
   };
}

#endif // COMPONENT_DOT_H
