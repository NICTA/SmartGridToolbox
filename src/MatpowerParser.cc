#include "MatpowerParser.h"

namespace SmartGridToolbox
{
   void MatpowerParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Bus : parse." << std::endl);

      assertFieldPresent(nd, "input_file");
      assertFieldPresent(nd, "network_name");

      string fName = state.expandName(nd["input_file"].as<std::string>());
      string netName = state.expandName(nd["network_name"].as<std::string>());
   }

   void MatpowerParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {

   }

}
