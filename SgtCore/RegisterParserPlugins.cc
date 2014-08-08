#include "RegisterParserPlugins.h"

#include "Parser.h"
#include "BusParser.h"
#include "CommonBranchParser.h"
#include "GenParser.h"
#include "MatpowerParser.h"
#include "ZipParser.h"

namespace SmartGridToolbox
{
   void registerParserPlugins(Parser& p)
   {
      p.registerParserPlugin<BusParser>();
      p.registerParserPlugin<CommonBranchParser>();
      p.registerParserPlugin<GenParser>();
      p.registerParserPlugin<MatpowerParser>();
      p.registerParserPlugin<ZipParser>();
   }
}
