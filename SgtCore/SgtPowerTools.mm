#import "SgtPowerTools.h"

#import "Common.h"
#import "Network.h"

#import <Foundation/Foundation.h>

#import <PowerTools/PowerTools.h>

using SmartGridToolbox::Complex;

#import <SgtCore.h>
#import "Network.h"

Network* smartGridToolbox2PowerTools(const SmartGridToolbox::Network& sgtNw)
{
   Network* nw = [[Network alloc] init]; // Network

   // Import names from SmartGridToolbox namespace.
   using SgtBus = SmartGridToolbox::Bus;
   using SgtCommonBranch = SmartGridToolbox::CommonBranch;
   using SgtGen = SmartGridToolbox::GenericGen;
   using SgtNetwork = SmartGridToolbox::Network;
   using SgtParser = SmartGridToolbox::Parser<Network>;
   using SgtZip = SmartGridToolbox::GenericZip;
   using SmartGridToolbox::Complex;

   // Translate into PowerTools structures.
   // TODO: factor out this code into a separate function.
   const int phase = 1;
   nw.mvaBase = sgtNw.PBase();
   for (auto& bus : sgtNw.busses())
   {
      const auto& zip = *bus->zips().begin();

      NSString* busId = [NSString stringWithUTF8String:(bus->id().c_str())];
      Complex PLoad = -sgtNw.S2Pu(zip->SConst()[0]);
      Complex yShunt = sgtNw.Y2Pu(zip->YConst()[0], bus->VBase());
      double VMin = sgtNw.V2Pu(bus->VMagMin(), bus->VBase());
      double VMax = sgtNw.V2Pu(bus->VMagMax(), bus->VBase());
      double kVBase = bus->VBase();

      Node* n = [[Node alloc] initWithId:busId load_p:PLoad.real() load_q:PLoad.imag() shunt_b:yShunt.imag() shunt_g:yShunt.real() v_min:VMin v_max:VMax kv_base:kVBase phase:phase];
      //        [n active] = bus->status();
      [nw addNode:n];
      for (const auto& gen : bus->gens())
      {
         // if (!gen->status()) continue;
         NSString* genId = [NSString stringWithUTF8String:(gen->id().c_str())];
         double PMin = sgtNw.S2Pu(gen->PMin());
         double PMax = sgtNw.S2Pu(gen->PMax());
         double QMin = sgtNw.S2Pu(gen->QMin());
         double QMax = sgtNw.S2Pu(gen->QMax());

         Gen* g = [[Gen alloc] initWithBus:n g_id:genId p_min:PMin p_max:PMax q_min:QMin q_max:QMax];
         g.active = gen->isInService() && [n active];

         Cost* c = [[Cost alloc] init];
         c.c0 = gen->c0();
         c.c1 = gen->c1();
         c.c2 = gen->c2();
         [g setCost:c];

         [n connect_gen:g];
         [nw addGen:n.gen.lastObject];
      }
   }

   for (const auto& branch : sgtNw.branches())
   {
      // if (!branch->status()) continue;
      const SgtCommonBranch& cBranch = dynamic_cast<SgtCommonBranch&>(*branch);
      const SgtBus& bus0 = *branch->bus0();
      const SgtBus& bus1 = *branch->bus1();

      NSString* id = [NSString stringWithUTF8String:(cBranch.id().c_str())];
      NSString* bus0Id = [NSString stringWithUTF8String:bus0.id().c_str()];
      NSString* bus1Id = [NSString stringWithUTF8String:bus1.id().c_str()];

      Node* node0 = [nw.nodeDict objectForKey:bus0Id];
      Node* node1 = [nw.nodeDict objectForKey:bus1Id];

      Complex ZSeries = sgtNw.Z2Pu(1.0/cBranch.YSeries(), bus1.VBase());
      Complex yShunt = sgtNw.Y2Pu(cBranch.YShunt(), bus1.VBase());
      double rateB = sgtNw.S2Pu(cBranch.rateA());
      Complex tap = cBranch.tapRatio()*bus1.VBase()/bus0.VBase();
      // Go from absolute to off-nominal tap ratio.

      Line* l = nil;
      if (abs(tap - Complex(1.0)) < 1e-6)
      {
         // Regular line.
         l = [[Line alloc] initWithId:id origin:node0 destination:node1 r:ZSeries.real() x:ZSeries.imag() charge:yShunt.imag() cap:rateB phase:phase];
      }
      else
      {
         // Transformer.
         double tapMag = abs(tap);
         double tapArg = arg(tap);
         l = [[Line alloc] initWithId:id origin:node0 destination:node1 r:ZSeries.real() x:ZSeries.imag() charge:yShunt.imag() cap:rateB tr:tapMag as:tapArg phase:phase];
      }
      l.active = cBranch.isInService() && [node0 active] && [node1 active];
      [nw addLine:l];
   }

   return nw;
}

void runOpf(SmartGridToolbox::Network& sgtNw)
{
   // std::cout << sgtNw << std::endl;
   Network* nw = smartGridToolbox2PowerTools(sgtNw);
   OcpModel* m = [[OcpModel alloc] initWithNet:nw];
   [m createRectACOPF];
   [m optimize];

   arma::Col<Complex> S(1);
   arma::Col<Complex> V(1);

   for (Node* nd in nw.nodes) {
      NSLog(@"%@\n    |V| = %f\n    theta = %f", nd.description, 
            nd.voltage.mag.floatValue, nd.voltage.theta.floatValue);
      auto sgtBus = sgtNw.bus(nd.b_id.UTF8String);
      V(0) = sgtNw.pu2V(std::polar(nd.voltage.mag.floatValue, nd.voltage.theta.floatValue), sgtBus->VBase());
      sgtBus->setV(V);
      std::cout << *sgtBus << std::endl;
   }
   for (Gen* gen in nw.gens) {
      NSLog(@"%@", gen.g_id);
      auto sgtGen = sgtNw.gen(gen.g_id.UTF8String);
      S(0) = sgtNw.pu2S(Complex{gen.pg.floatValue, gen.qg.floatValue});
      sgtGen->setInServiceS(S);
   }
   [nw dumpToLog];
   // std::cout << "--------------------------------------------------------------------------------" << std::endl;
   // std::cout << sgtNw << std::endl;
}
