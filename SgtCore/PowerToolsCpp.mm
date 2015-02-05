#import "PowerToolsCpp.h"

#import <SgtCore/Common.h>

#import <Foundation/Foundation.h>

#import <PowerTools/PowerTools.h>

using SmartGridToolbox::Complex;

void runOpf(SmartGridToolbox::Network& sgtNw)
{
   // std::cout << sgtNw << std::endl;
   Network* nw = smartGridToolbox2PowerTools(sgtNw);
   OcpModel* m = [[OcpModel alloc] initWithNet:nw];
   [m createDistflowOPF];
   [m optimize];

   arma::Col<Complex> S(1);
   arma::Col<Complex> V(1);

   for (Node* nd in nw.nodes) {
      // NSLog(@"%@\n    |V| = %f\n    theta = %f", nd.description, 
      //       nd.voltage.mag.floatValue, nd.voltage.theta.floatValue);
      auto sgtBus = sgtNw.bus(nd.b_id.UTF8String);
      V[0] = sgtNw.pu2V(std::polar(nd.voltage.mag.floatValue, nd.voltage.theta.floatValue), sgtBus->VBase());
      sgtBus->setV(V);
   }
   for (Gen* gen in nw.gens) {
      // NSLog(@"%@", gen.g_id);
      auto sgtGen = sgtNw.gen(gen.g_id.UTF8String);
      S[0] = sgtNw.pu2S(Complex{gen.pg.floatValue, gen.qg.floatValue});
      sgtGen->setInServiceS(S);
   }
   // [nw dumpToLog];
   // std::cout << "--------------------------------------------------------------------------------" << std::endl;
   // std::cout << sgtNw << std::endl;
}
