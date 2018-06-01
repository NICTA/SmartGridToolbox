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

#include <SgtCore.h>
#include <SgtSim.h>

using namespace Sgt;
using namespace std;

// Please start by skipping ahead to the main(...) function.

namespace Sgt
{
    class SillyPlugin: public ParserPlugin<Network>
    {
        public:
        virtual const char* key() const override {return "silly";} // Parse YAML under the "silly" key.

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override
        {
            // Asserts that various fields are present:
            assertFieldPresent(nd, "silly_string");
            assertFieldPresent(nd, "silly_int");

            // Parser the YAML in node nd:
            string sillyString = parser.expand<string>(nd["silly_prefix"]);
            int sillyInt = parser.expand<int>(nd["silly_int"]);

            // Create prefix based on YAML:
            string sillyPrefix = sillyString + "_" + to_string(sillyInt) + "_";

            // Modify the network by giving all buses a "silly name" in their user data JSON.
            for (auto b : netw.buses()) b->userData()["silly_name"] = sillyPrefix + b->id();
        }
    };
}

int main(int argc, char** argv)
{
    {
        // An empty network:
        Network netw;
        sgtLogMessage() << "Before parsing: " << endl;
        sgtLogMessage() << netw << endl;

        // Create a parser that is able to modify netw based on a YAML config:
        Parser<Network> p; // An alias for this is "NetworkParser".

        // Parse a yaml file into netw;
        p.parse("network.yaml", netw);
        sgtLogMessage() << "After parsing: " << endl;
        sgtLogMessage() << netw << endl;

        // Now we'll show how to add custom parsing code.
        // Each Parser<T> has a corresponding registerParserPlugins<>(Parser<T>&) function that registers a standard
        // set of plugins. For example, Parser<Network> registers BusParserPlugin, BranchParserPlugin, etc.
        // You can also register individual plugins user Parser<T>::registerParserPlugin(), like below:
        p.registerParserPlugin<SillyPlugin>();

        // Lets try it now:
        Network netwB;
        p.parse("network.yaml", netwB);
        sgtLogMessage() << "After parsing (including SillyPlugin): " << endl;
        sgtLogMessage() << netwB << endl;
        for (const auto& b : netwB.buses())
        {
            std::cout << "Bus " << b->id() << " user data : " << b->userData().dump() << endl;
        }
    }
}
