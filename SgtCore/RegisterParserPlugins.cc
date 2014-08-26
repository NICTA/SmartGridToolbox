#include "Parser.h"
#include "BusParser.h"
#include "CommonBranchParser.h"
#include "GenericBranchParser.h"
#include "GenParser.h"
#include "MatpowerParser.h"
#include "ZipParser.h"

namespace SmartGridToolbox
{
   class Network;
   template<> void registerParserPlugins<Network>(Parser<Network>& p)
   {
std::cout << "Register" << std::endl;
      p.registerParserPlugin<BusParser>();
      p.registerParserPlugin<CommonBranchParser>();
      p.registerParserPlugin<GenericBranchParser>();
      p.registerParserPlugin<GenParser>();
      p.registerParserPlugin<MatpowerParser>();
      p.registerParserPlugin<ZipParser>();
   }
}
