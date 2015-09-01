// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Event.h"

#include <SgtCore/Common.h>

namespace Sgt
{
    void Event::trigger()
    {
        if (isEnabled_)
        {
SGT_DEBUG(
            sgtLogDebug() << "Event was triggered: " << description_ << std::endl;
            {
            LogIndent _
);
            for (const Action& action : actions_)
            {
SGT_DEBUG(
                    sgtLogDebug() << "Event perform action: " << action.description() << std::endl;
                    {
                        LogIndent _
);
                action.perform();
SGT_DEBUG(
                    }
                    sgtLogDebug() << "Event performed action: " << action.description() << std::endl
);
                }
SGT_DEBUG(
            }
            sgtLogDebug() << "Event finished: " << description_ << std::endl
);
        }
        else
        {
SGT_DEBUG(
            sgtLogDebug() << "Event not triggered because it is not enabled: "
                          << description_ << std::endl;
);
        }
    }
}
