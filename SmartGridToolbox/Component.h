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
   /** Component derived classes are expected to override three methods:
    *
    *  virtual Time validUntil() const, 
    *  virtual void initializeState(),
    *  virtual void updateState(Time t).
    *
    *  validUntil() tells the simulation when the component will require its next scheduled update. Note that it may
    *  be called on to update before this time, due to the requirements of other components that depend on it.
    *
    *  initialiseState() does any required initial work, before the simulation has even started. At this point, the
    *  timestep of the component will be at negative infinity (SmartGridToolbox::posix_time::neg_infin).
    *
    *  updateState(t) updates the state from the current time to the next scheduled time t. Note that at the time the
    *  function is called, the current time of a Component comp is comp.time(), which will advance to t until
    *  directly after the function returns. */
   class Component
   {
      public:
         /// @name Lifecycle
         /// @{
         
         /// Constructor.
         explicit Component(const std::string & name); 

         /// Destructor.
         virtual ~Component() = 0;

         /// @}

         /// @name Name
         /// @{
         
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
         
         /// @}
         
         /// @name Timestepping
         /// @{
         
         /// Get the current step for the object.
         Time time() const
         {
            return time_;
         }

         /// @}
         
         /// @name Rank
         /// @brief Rank: A < B means B depends on A, not vice-versa, so A should go first.
         /// @{

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

         const std::vector<const Component*> & dependencies() const
         {
            return dependencies_;
         }

         /// Components on which I depend will update first.
         void dependsOn(const Component & b)
         {
            dependencies_.push_back(&b);
         }

         /// @}

         /// @name Simulation
         /// @{

         /// Initialize state of the object.
         /** This simply does the initial work needed to set the object up, prior to simulation. The time following
          *  initialization will be set to negative infinity, and the object will not be considered to be in an
          *  invalid state. To progress to a valid state, the object will need to undergo an update(). */
         void initialize();

         /// Bring state up to time t.
         void update(Time t);

         virtual Time validUntil() const = 0
         {
            return posix_time::pos_infin;
         }
         
         /// @}

         /// @name Events
         /// @{

         /// Triggered just before my update.
         Event & willUpdate() {return willUpdate_;}
         
         /// Triggered after my update.
         Event & didUpdate() {return didUpdate_;}
         
         /// Triggered when I am flagged for future update.
         Event & needsUpdate() {return needsUpdate_;}
         
         /// Triggered when I am about to update to a new timestep. 
         Event & willStartNewTimestep() {return willStartNewTimestep_;}
         
         /// Triggered when I just updated, completing the current timestep. 
         Event & didCompleteTimestep() {return didCompleteTimestep_;}
         /// @}

         /// @}

      protected:
         typedef std::vector<const Component*> ComponentVec;

      protected:
         /// Reset state of the object, time will be at negative infinity.
         virtual void initializeState()
         {
            // Empty.
         }

         /// Bring state up to time current time.
         virtual void updateState(Time t)
         {
            // Empty.
         }
         
      private:
         std::string name_;
         Time time_;                 ///< The current time.
         ComponentVec dependencies_; ///< I depend on these.
         int rank_;                  ///< Evaluation rank, based on weak ordering.
         Event willUpdate_;
         Event didUpdate_;
         Event needsUpdate_;
         Event willStartNewTimestep_;
         Event didCompleteTimestep_;
   };

inline virtual Component::~Component() {}

}

#endif // COMPONENT_DOT_H
