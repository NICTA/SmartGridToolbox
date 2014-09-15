#ifndef SIM_COMPONENT_DOT_H
#define SIM_COMPONENT_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/Event.h>

namespace SmartGridToolbox
{
   class SimComponentAbc : virtual public ComponentInterface
   {
      public:

      /// @name Lifecycle
      /// @{
     
         SimComponentAbc() = default;

         virtual ~SimComponentAbc() = default;

      /// @}

      /// @name Component Type:
      /// @{
         
         static constexpr const char* sComponentType()
         {
            return "sim_component";
         }

      /// @}
      
      /// @name Simulation flow
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

         const std::vector<std::weak_ptr<const SimComponentAbc>>& dependencies() const
         {
            return dependencies_;
         }

         /// @brief Components on which I depend will update first.
         void dependsOn(std::shared_ptr<const SimComponentAbc> b)
         {
            dependencies_.push_back(b);
         }

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

         Time time_{posix_time::not_a_date_time}; ///< The time to which I am up to date
         std::vector<std::weak_ptr<const SimComponentAbc>> dependencies_; ///< I depend on these.
         int rank_{-1}; ///< Evaluation rank, based on weak ordering.
         Event willUpdate_{std::string(sComponentType()) + "Will update"}; ///< Triggered immediately prior to upddate. 
         Event didUpdate_{std::string(sComponentType()) + "Did update"}; ///< Triggered immediately post update.
         Event needsUpdate_{std::string(sComponentType()) + "Needs update"}; ///< Triggered when I need to be updated.
         Event willStartNewTimestep_{std::string(sComponentType()) + "Will start new timestep"};
            ///< Triggered immediately prior to time advancing.
         Event didCompleteTimestep_{std::string(sComponentType()) + "Did complete timestep"}; ///< Triggered just after fully completing a timestep.
   };

   class SimComponent : public SimComponentAbc, public Component
   {
      public:
         SimComponent(const std::string& id) : Component(id) {}
         virtual const char* componentType() const override
         {
            return sComponentType();
         }
   };
}

#endif // SIM_COMPONENT_DOT_H
