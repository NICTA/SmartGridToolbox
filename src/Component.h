#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <string>
#include <vector>
#include <map>
#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Event.h>

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
         explicit Component(const std::string & name); 

         /// Destructor.
         virtual ~Component()
         {
            // Empty.
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
         Time startTime() const
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

         const std::vector<const Component *> & dependencies() const
         {
            return dependencies_;
         }

         /// Components on which I depend will update first.
         void dependsOn(const Component & b)
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

         /// Bring state up to time t if it is not already there.
         /** @param t the Time to advance to. */
         void ensureAtTime(Time t);

         virtual Time validUntil() const
         {
            return posix_time::pos_infin;
         }

         /// @name Events
         /// @{
         Event & willUpdate() {return willUpdate_;}
         Event & didUpdate() {return didUpdate_;}
         Event & needsUpdate() {return needsUpdate_;}
         Event & willStartNewTimestep() {return willStartNewTimestep_;}
         Event & didCompleteTimestep() {return didCompleteTimestep_;}
         /// @}

         /// @}

      protected:
         typedef std::vector<const Component *> ComponentVec;

      private:
         /// Reset state of the object, time is at currentTime_.
         virtual void initializeState()
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
         Time startTime_; ///< The initial time.
         Time currentTime_; ///< The current time.
         ComponentVec dependencies_; ///< I depend on these.
         int rank_;  ///< Evaluation rank, based on weak ordering.
         Event willUpdate_;
         Event didUpdate_;
         Event needsUpdate_;
         Event willStartNewTimestep_;
         Event didCompleteTimestep_;
   };
}

#endif // COMPONENT_DOT_H
