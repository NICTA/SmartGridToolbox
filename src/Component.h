#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <string>
#include <vector>
#include "Common.h"

namespace SmartGridToolbox
{
   /// Base class for simulation objects.
   class Component
   {
      public:
         /// Constructor.
         /** @param name the unique name */
         explicit Component(const std::string & name) : name_(name)
         {
            // Empty.
         }

         /// Pure virtual destructor.
         virtual ~Component()
         {
            // Empty.
         }

         /// Get the name of the object.
         const std::string & GetName() const
         {
            return name_;
         }
         
         /// Get the current step for the object.
         ptime GetTimestamp()
         {
            return t_;
         }

         /// Reset state of the object, time to timestamp t.
         /** @param t */
         virtual ptime Initialize(const ptime t = not_a_date_time)
         {
            t_ = t;
            return not_a_date_time;
         }

         /// Bring state up to time t.
         /** @param t the timestamp to advance to. */
         virtual void AdvanceToTime(ptime t) = 0;

         /// Dependents update after I update.
         void AddDependent(Component & dependent)
         {
            dependents_.push_back(&dependent);
         }

      private:
         std::string name_;
         ptime t_;
         std::vector<Component *> dependents_;
         int rank_;  // Evaluation rank, based on weak ordering.
   };
}

#endif // COMPONENT_DOT_H
