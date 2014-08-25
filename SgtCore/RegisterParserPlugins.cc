#include "Parser.h"
#include "BusParser.h"
#include "CommonBranchParser.h"
#include "GenParser.h"
#include "MatpowerParser.h"
#include "ZipParser.h"

namespace SmartGridToolbox
{
   class Network;
   template<> void registerParserPlugins<Network>(Parser<Network>& p)
   {
std::cout << "Register" << std::endl;
      p.registerParserPlugin(std::unique_ptr<BusParser>(new BusParser()));
      p.registerParserPlugin(std::unique_ptr<CommonBranchParser>(new CommonBranchParser()));
      p.registerParserPlugin(std::unique_ptr<GenParser>(new GenParser()));
      p.registerParserPlugin(std::unique_ptr<MatpowerParser>(new MatpowerParser()));
      p.registerParserPlugin(std::unique_ptr<ZipParser>(new ZipParser()));
   }
}
