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
         explicit Component(const std::string & name) : name_(name),
                                                        rank_(-1)
         {
            // Empty.
         }

         /// Pure virtual destructor.
         virtual ~Component()
         {
            // Empty.
         }

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

         /// My dependencies.
         const std::vector<Component *> & getDependencies() const
         {
            return dependencies_;
         }
         /// Dependents update after I update.
         void addDependency(Component & b)
         /** @param t Component on which I depend. */
         {
            dependencies_.push_back(&b);
         }

         /// Reset state of the object, time to timestamp t.
         /** @param t */
         virtual void initialize(const ptime t = not_a_date_time)
         {
            t_ = t;
         }

         /// Bring state up to time t.
         /** @param t the timestamp to advance to. */
         virtual void advanceToTime(ptime t)
         {
            t_ = t;
         }

      private:
         std::string name_;
         ptime t_;
         std::vector<Component *> dependencies_;
         int rank_;  // Evaluation rank, based on weak ordering.
   };
}

#endif // COMPONENT_DOT_H
