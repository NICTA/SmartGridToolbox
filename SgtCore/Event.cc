#include "Event.h"

#include <SgtCore/Common.h>

namespace Sgt
{
    void Event::trigger()
    {
        if (isEnabled_)
        {
            SGT_DEBUG(
                Log().debug() << "Event was triggered: " << description_ << std::endl;
            {
                LogIndent _
                );
                for (const Action& action : actions_)
            {
                SGT_DEBUG(
                    Log().debug() << "Event perform action: " << action.description() << std::endl;
                {
                    LogIndent _
                    );
                    action.perform();
                    SGT_DEBUG(
                }
                Log().debug() << "Event performed action: " << action.description() << std::endl
                );
                }
                SGT_DEBUG(
            }
            Log().debug() << "Event finished: " << description_ << std::endl
            );
        }
        else
        {
            SGT_DEBUG(
                Log().debug() << "Event not triggered because it is not enabled: "
                << description_ << std::endl;
            {
                LogIndent _
                );
            }
        }
    }
