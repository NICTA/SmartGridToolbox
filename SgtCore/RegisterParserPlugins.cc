#include "Parser.h"
#include "BusParser.h"
#include "CommonBranchParser.h"
#include "DgyTransformerParser.h"
#include "GenericBranchParser.h"
#include "GenericGenParser.h"
#include "GenericZipParser.h"
#include "MatpowerParser.h"
#include "OverheadLineParser.h"

namespace SmartGridToolbox
{
   class Network;
   template<> void registerParserPlugins<Network>(Parser<Network>& p)
   {
      p.registerParserPlugin<BusParser>();
      p.registerParserPlugin<CommonBranchParser>();
      p.registerParserPlugin<DgyTransformerParser>();
      p.registerParserPlugin<GenericBranchParser>();
      p.registerParserPlugin<GenericGenParser>();
      p.registerParserPlugin<GenericZipParser>();
      p.registerParserPlugin<MatpowerParser>();
      p.registerParserPlugin<OverheadLineParser>();
   }
}
