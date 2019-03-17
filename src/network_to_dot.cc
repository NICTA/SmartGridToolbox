// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#include <iostream>
#include <fstream>

using namespace Sgt;

int main(int argc, const char** argv)
{
    if (argc < 3)
    {
        sgtError("Usage: " << argv[0] << " config_name output_name [option ...].");
    }

    const char* configName = argv[1];
    const char* outName = argv[2];
    int nOpt = argc - 3;
    const char** opts = argv + 3;

    std::ofstream out(outName);

    Simulation sim;
    Parser<Simulation> p;
    p.parse(configName, sim);
    sim.initialize();

    auto netw = sim.simComponent<SimNetwork>("network")->network();

    out << "digraph G {" << std::endl;
    {
        StreamIndent _(out);
        out << "node [shape=circle, width=1, fixedsize=true, style=filled];" << std::endl;
        out << "edge [len=2.5];" << std::endl;
        for (std::size_t i = 0; i < nOpt; ++i)
        {
            out << opts[i] << ";" << std::endl;
        }
        for (auto& nd : netw->nodes())
        {
            const auto& bus = *nd->bus();
            std::string col;
            switch (bus.type())
            {
                case BusType::SL:
                    col = "deeppink";
                    break;
                case BusType::PV:
                    col = "orange";
                    break;
                case BusType::PQ:
                    col = "lightblue";
                    break;
                default:
                    col = "black";
                    break;
            }
            out << bus.id() << " [" << "color=" << col << "];" << std::endl;
            for (auto& gen : nd->gens())
            {
                out << gen->id() << " [shape=point, width=0.1, color=orange" << "];" << std::endl;
                out << gen->id() << " -> " << bus.id() << " [len=1, color=orange];" << std::endl;
            }
            for (auto& zip : nd->zips())
            {
                out << zip->id() << " [shape=point, width=0.1, color=lightblue];" << std::endl;
                out << bus.id() << " -> " << zip->id() << " [len=1, color=lightblue];" << std::endl;
            }
        }
        for (auto& arc : netw->arcs())
        {
            // out << arc->node0()->bus()->id() << " -> " << arc->node1()->bus()->id()
            //   << " [label=" << arc->branch()->id() << ", dir=none];" << std::endl;
            out << arc->node0()->bus()->id() << " -> " << arc->node1()->bus()->id()
                << " [tooltip=" << arc->branch()->id() << ", dir=none];" << std::endl;
        }
    }
    out << "}" << std::endl;
    out.close();
}
