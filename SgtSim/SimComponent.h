#ifndef SIM_COMPONENT_DOT_H
#define SIM_COMPONENT_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/Event.h>

namespace SmartGridToolbox
{
   class SimComponentInterface : virtual public ComponentInterface
   {
      public:
      
      /// @name Lifecycle
      /// @{
     
         virtual ~SimComponentInterface() {}

      /// @}
      
      /// @name Virtual methods to be overridden by derived classes.
      /// @{
      
      public:

         /// @brief What is the latest time that I should update?
         virtual Time validUntil() const = 0;

      protected:

         /// @brief Reset state of the object, time will be at negative infinity.
         virtual void initializeState() = 0;

         /// @brief Bring state up to time current time.
         virtual void updateState(Time t) = 0;

      /// @}
      
      public:

      /// @name Timestepping
      /// @{

         /// @brief Get the current step for the object.
         virtual Time time() const = 0;

      /// @}

      /// @name Rank
      /// @brief Rank: A < B means B depends on A, not vice-versa, so A should go first.
      /// @{

         /// @brief Get the rank of the object.
         virtual int rank() const = 0;

         /// @brief Set the rank of the object.
         virtual void setRank(int rank) = 0;

      /// @}

      /// @name Dependencies.
      /// @{

         virtual const std::vector<std::weak_ptr<const SimComponentInterface>>& dependencies() const = 0;

         /// @brief Components on which I depend will update first.
         virtual void dependsOn(const std::shared_ptr<SimComponentInterface> b) = 0;

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
         virtual Event& willUpdate() = 0;

         /// @brief Triggered after my update.
         virtual Event& didUpdate() = 0;

         /// @brief Triggered when I am flagged for future update.
         virtual Event& needsUpdate() = 0;

         /// @brief Triggered when I am about to update to a new timestep.
         virtual Event& willStartNewTimestep() = 0;

         /// @brief Triggered when I just updated, completing the current timestep.
         virtual Event& didCompleteTimestep() = 0;

      /// @}
   };

   class SimComponentAbc : virtual public SimComponentInterface
   {
      /// @name Virtual methods to be overridden by derived classes.
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
     
         SimComponentAbc();

      /// @}

      /// @name Timestepping
      /// @{

         /// @brief Get the current step for the object.
         virtual Time time() const
         {
            return time_;
         }

      /// @}

      /// @name Rank
      /// @brief Rank: A < B means B depends on A, not vice-versa, so A should go first.
      /// @{

         /// @brief Get the rank of the object.
         virtual int rank() const
         {
            return rank_;
         }

         /// @brief Set the rank of the object.
         virtual void setRank(int rank)
         {
            rank_ = rank;
         }

      /// @}

      /// @name Dependencies.
      /// @{

         virtual const std::vector<std::weak_ptr<const SimComponentInterface>>& dependencies() const
         {
            return dependencies_;
         }

         /// @brief Components on which I depend will update first.
         virtual void dependsOn(const std::shared_ptr<SimComponentInterface> b)
         {
            dependencies_.push_back(b);
         }

      /// @}

      /// @name Events
      /// @{

         /// @brief Triggered just before my update.
         virtual Event& willUpdate() {return willUpdate_;}

         /// @brief Triggered after my update.
         virtual Event& didUpdate() {return didUpdate_;}

         /// @brief Triggered when I am flagged for future update.
         virtual Event& needsUpdate() {return needsUpdate_;}

         /// @brief Triggered when I am about to update to a new timestep.
         virtual Event& willStartNewTimestep() {return willStartNewTimestep_;}

         /// @brief Triggered when I just updated, completing the current timestep.
         virtual Event& didCompleteTimestep() {return didCompleteTimestep_;}

      /// @}

      private:

         Time time_ = posix_time::not_a_date_time; ///< The time to which I am up to date
         std::vector<std::weak_ptr<const SimComponentInterface>> dependencies_; ///< I depend on these.
         int rank_ = -1; ///< Evaluation rank, based on weak ordering.
         Event willUpdate_; ///< Triggered immediately prior to upddate. 
         Event didUpdate_; ///< Triggered immediately post update.
         Event needsUpdate_; ///< Triggered when I need to be updated.
         Event willStartNewTimestep_; ///< Triggered immediately prior to time advancing.
         Event didCompleteTimestep_; ///< Triggered immediately after fully completing a timestep.
   };

   class SimComponent : public SimComponentAbc, public Component
   {
      public:
         SimComponent(const std::string& id) : Component(id) {}
   };
}

#endif // SIM_COMPONENT_DOT_H
