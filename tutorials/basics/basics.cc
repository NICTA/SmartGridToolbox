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

#include <SgtCore.h> // Include SgtCore headers.

// Please note the following namespaces:
using namespace Sgt;
using namespace std;
using namespace arma;

int main(int argc, char** argv)
{
    {
        sgtLogMessage() << "----------------" << endl;
        sgtLogMessage() << "Logging and Errors:" << endl;
        sgtLogMessage() << "----------------" << endl;
        sgtLogMessage() << "This is how we log a message in SmartGridToolbox" << endl;
        sgtLogWarning() << "This is how we log a warning in SmartGridToolbox" << endl;
        {
            sgtLogIndent();
            sgtLogMessage() << "This scope will be indented." << endl;
        }
        sgtLogMessage() << "This message will not not be indented." << endl;
        sgtLogMessage(LogLevel::VERBOSE) << "This will not be logged" << endl;
        messageLogLevel() = LogLevel::VERBOSE;
        sgtLogMessage(LogLevel::VERBOSE) << "This will be logged" << endl;
        messageLogLevel() = LogLevel::NONE;
        sgtLogMessage(LogLevel::VERBOSE) << "This will not be logged" << endl;
        messageLogLevel() = LogLevel::NORMAL;
        sgtLogMessage(LogLevel::VERBOSE) << "This will be logged" << endl;
        sgtAssert(true == true, "Assertion error message");
        if (false) sgtError("There was an error! I will throw an exception.");
    }

    {
        sgtLogMessage() << "----------------" << endl;
        sgtLogMessage() << "Complex Numbers:" << endl;
        sgtLogMessage() << "----------------" << endl;
        Complex c{4.0, 5.0};
        sgtLogMessage() << "Complex c = " << c << endl;
        sgtLogIndent();
        sgtLogMessage() << "real part = " << c.real() << endl;
        sgtLogMessage() << "imag part = " << c.imag() << endl;
        sgtLogMessage() << "abs = " << abs(c) << endl;
        sgtLogMessage() << "conj = " << conj(c) << endl;
    }
    
    {
        sgtLogMessage() << "----------------" << endl;
        sgtLogMessage() << "Linear Algebra:" << endl;
        sgtLogMessage() << "----------------" << endl;
        sgtLogMessage() << "SmartGridToolbox uses Armadillo for linear algebra." << endl;
        sgtLogMessage() << "Please see the Armadillo documentation." << endl;
        Col<double> v{1.0, 2.0, 3.0, 3.0};
        sgtLogMessage() << "Real vector:" << endl << v << endl;
        Mat<Complex> m{{{1.0, 0.0}, {2.0, 1.0}}, {{3.0, 10.0}, {4.0, 20.0}}};
        sgtLogMessage() << "Complex matrix:" << endl << m << endl;
    }

    {
        sgtLogMessage() << "----------------" << endl;
        sgtLogMessage() << "Time:" << endl;
        sgtLogMessage() << "----------------" << endl;
        
        sgtLogMessage() << 
            "SmartGridToolbox uses boost::posix_time::time_duration to represent both durations and absolute times."
            << endl;

        sgtLogMessage() << "Setting the global timezone using a timezone/DST rule." << endl;
        Sgt::timezone() = Timezone("AEST10AEDT,M10.5.0/02,M3.5.0/03");

        Time t1 = seconds(6); 
        sgtLogMessage() << "t1 = " << t1 << endl;

        Time t2 = minutes(6); 
        sgtLogMessage() << "t2 = " << t2 << " " << dSeconds(t2) << endl;
        
        Time t3 = timeFromDSeconds(124.8); 
        sgtLogMessage() << "t3 = " << t3 << " " << dSeconds(t3) << endl;

        Time t4 = timeFromUtcTimeString("2018-04-07 20:00:00");
        sgtLogMessage() << "t4 = " << localTimeString(t4) << endl;
        sgtLogMessage() << "Note formatting as a duration. For time formatting, do this:" << endl;
        sgtLogMessage() << "t4 = " << localTimeString(t4) << endl;

        Time t5 = timeFromLocalTimeString("2018-04-07 20:00:00");
        sgtLogMessage() << "t5 = " << localTimeString(t5) << endl;

        Time t6 = TimeSpecialValues::pos_infin;
        Time t7 = TimeSpecialValues::neg_infin;
        Time t8 = TimeSpecialValues::not_a_date_time;
        sgtLogMessage() << "t6 = " << t6 << endl;
        sgtLogMessage() << "t7 = " << t7 << endl;
        sgtLogMessage() << "t8 = " << t8 << endl;
    }

    {
        sgtLogMessage() << "----------------" << endl;
        sgtLogMessage() << "JSON:" << endl;
        sgtLogMessage() << "----------------" << endl;
        sgtLogMessage() << "JSON in SmartGridToolbox is handled using the nlohmann::json library." << endl;
        sgtLogMessage() << "Please see the documentation for this library." << endl;
        sgtLogMessage() << "We use the alias Sgt::json = nlohmann::json." << endl;
        json obj = {{"key_1", 1}, {"key_2", {2, 3, 4, nullptr}}};
        sgtLogMessage() << "obj = " << obj.dump(2) << endl;
    }
}
