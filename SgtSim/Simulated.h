#ifndef SIMULATED_DOT_H
#define SIMULATED_DOT_H

#include <SgtSim/Event.h>

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>

namespace SmartGridToolbox
{
   class Simulated : public HasId
   {
      /// @name Overridden from HasId (normally via sister class delegation).
      /// @{
      
      public:

         virtual const std::string& id() const;

      /// @}
      
      /// @name Member functions which derived classes may override.
      /// @{
      
      public:

         /// @brief What is the latest time that I should update?
         virtual Time validUntil() const
         {
            return posix_time::pos_infin;
         }

      protected:

         /// @brief Reset state of the object, time will be at negative infinity.
         virtual void initializeState()
         {
            // Empty.
         }

         /// @brief Bring state up to time current time.
         virtual void updateState(Time t)
         {
            // Empty.
         }

      /// @}
      
      public:
      
      /// @name Lifecycle
      /// @{
     
         virtual ~Simulated() = default;

      /// @}

      /// @name Timestepping
      /// @{

         /// @brief Get the current step for the object.
         Time time() const
         {
            return time_;
         }

      /// @}

      /// @name Rank
      /// @brief Rank: A < B means B depends on A, not vice-versa, so A should go first.
      /// @{

         /// @brief Get the rank of the object.
         int rank() const
         {
            return rank_;
         }

         /// @brief Set the rank of the object.
         void setRank(int rank)
         {
            rank_ = rank;
         }

      /// @}

      /// @name Dependencies.
      /// @{

         const std::vector<const Simulated*>& dependencies() const
         {
            return dependencies_;
         }

         /// @brief Components on which I depend will update first.
         void dependsOn(const Simulated& b)
         {
            dependencies_.push_back(&b);
         }

      /// @}

      /// @name Simulation
      /// @{

         /// @brief Initialize state of the object.
         ///
         /// This simply does the initial work needed to set the object up, prior to simulation. The time following
         /// initialization will be set to negative infinity, and the object will not be considered to be in an
         /// invalid state. To progress to a valid state, the object will need to undergo an update().
         void initialize();

         /// @brief Bring state up to time t.
         void update(Time t);

      /// @}

      /// @name Events
      /// @{

         /// @brief Triggered just before my update.
         Event& willUpdate() {return willUpdate_;}

         /// @brief Triggered after my update.
         Event& didUpdate() {return didUpdate_;}

         /// @brief Triggered when I am flagged for future update.
         Event& needsUpdate() {return needsUpdate_;}

         /// @brief Triggered when I am about to update to a new timestep.
         Event& willStartNewTimestep() {return willStartNewTimestep_;}

         /// @brief Triggered when I just updated, completing the current timestep.
         Event& didCompleteTimestep() {return didCompleteTimestep_;}

      /// @}

      private:

         Time time_;                                     ///< The time to which I am up to date
         std::vector<const Simulated*> dependencies_; ///< I depend on these.
         int rank_;                                      ///< Evaluation rank, based on weak ordering.
         Event willUpdate_;                              ///< Triggered immediately prior to upddate. 
         Event didUpdate_;                               ///< Triggered immediately post update.
         Event needsUpdate_;                             ///< Triggered when I need to be updated.
         Event willStartNewTimestep_;                    ///< Triggered immediately prior to time advancing.
         Event didCompleteTimestep_;                     ///< Triggered immediately after fully completing a timestep.
   };
}

#endif // SIMULATED_DOT_H
