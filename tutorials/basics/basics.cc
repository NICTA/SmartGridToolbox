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
    }
}
