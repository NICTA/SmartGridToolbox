//
//  LibPowerToolsCpp.m
//  LibPowerToolsCpp
//
//  Created by Dan Gordon on 30/07/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#import "PowerToolsCpp.h"

#import <Foundation/Foundation.h>

#import <SgtCore/Common.h>
#import <PowerTools/PowerTools.h>

using SmartGridToolbox::Complex;

void runOpf(SmartGridToolbox::Network& lpfNw)
{
    std::cout << lpfNw << std::endl;
    Network* nw = libPowerFlow2PowerTools(lpfNw);
    OcpModel* m = [[OcpModel alloc] initWithNet:nw];
    [m createDistflowOPF];
    [m optimize];
    
    ublas::vector<Complex> S(1);
    ublas::vector<Complex> V(1);
    
    for (Node* nd in nw.nodes) {
        NSLog(@"%@\n    |V| = %f\n    theta = %f", nd.description, nd.voltage.floatValue, nd.theta.floatValue);
        LibPowerFlow::Bus* lpfBus = lpfNw.bus(nd.b_id.UTF8String);
        V[0] = lpfNw.pu2V(std::polar(nd.voltage.floatValue, nd.theta.floatValue), lpfBus->VBase());
        lpfBus->setV(V);
    }
    for (Gen* gen in nw.gens) {
        NSLog(@"%@", gen.g_id);
        LibPowerFlow::Gen* lpfGen = lpfNw.gen(gen.g_id.UTF8String);
        S[0] = lpfNw.pu2S(LibPowerFlow::Complex{gen.pg.floatValue, gen.qg.floatValue});
        lpfGen->setS(S);
    }
    [nw dumpToLog];
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << lpfNw << std::endl;
}
