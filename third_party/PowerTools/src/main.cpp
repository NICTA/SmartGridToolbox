//
//  main.cpp
//  MP2DAT
//
//  Created by Hassan Hijazi on 14/01/13.
//  Copyright (c) 2013 NICTA. All rights reserved.
//

#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include "PowerTools++/Net.h"
#include "PowerTools++/IpoptProgram.h"
#include "PowerTools++/Solver.h"
#include "PowerTools++/Complex.h"
#include "PowerTools++/PowerModel.h"

using namespace std;

//  Windows
#ifdef _WIN32
#include <Windows.h>
double get_wall_time(){
    LARGE_INTEGER time,freq;
    if (!QueryPerformanceFrequency(&freq)){
        //  Handle error
        return 0;
    }
    if (!QueryPerformanceCounter(&time)){
        //  Handle error
        return 0;
    }
    return (double)time.QuadPart / freq.QuadPart;
}
double get_cpu_time(){
    FILETIME a,b,c,d;
    if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return
        (double)(d.dwLowDateTime |
                 ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }else{
        //  Handle error
        return 0;
    }
}

//  Posix/Linux
#else
#include <time.h>
#include <sys/time.h>
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time(){
    return (double)clock() / CLOCKS_PER_SEC;
}
#endif

int main (int argc, const char * argv[])
{
//    if (argc != 4) {
//        cerr << "Wrong number of arguments.\n";
//        exit(1);
//    }
//    PowerModelType pmt = ACPOL;
//    PowerModelType pmt = SOCP;
    //  Start Timers
    double wall0 = get_wall_time();
    double cpu0  = get_cpu_time();
    PowerModelType pmt = SDP;
//    PowerModelType pmt = QC;
//    PowerModelType pmt = QC_SDP;
//    PowerModelType pmt = SOCP_OTS;
//    PowerModelType pmt = ACRECT;
    SolverType st = ipopt;
//    SolverType st = gurobi;
//        string filename = "../../data/nesta_case5_pjm__sad.m";
//    string filename = "../../data/nesta_case3_lmbd.m";
    string filename = "../../data/nesta_case118_ieee.m";
//    string filename = "../../data/nesta_case29_edin__sad.m";
//    string filename = "../../data/nesta_case24_ieee_rts.m";
//    string filename = "../../data/nesta_case24_ieee_rts__api.m";
//    string filename = "../../data/nesta_case30_ieee.m";
//    string filename = "../../data/nesta_case57_ieee.m";
//    string filename = "../../data/nesta_case30_as__sad.m";
//    string filename = "../../data/nesta_case24_ieee_rts__sad.m";
//    string filename = "../../data/nesta_case2224_edin.m";
//    string filename = "../../data/nesta_case14_ieee.m";
//    string filename = "../../data/nesta_case162_ieee_dtc.m";
//    string filename = "../../data/nesta_case5_pjm.m";
    if (argc >=2) {
        filename = argv[1];
    
        if(!strcmp(argv[2],"ACPOL")) pmt = ACPOL;
        else if(!strcmp(argv[2],"ACRECT")) pmt = ACRECT;
        else if(!strcmp(argv[2],"QC")) pmt = QC;
        else if(!strcmp(argv[2],"QC_SDP")) pmt = QC_SDP;
        else if(!strcmp(argv[2],"OTS")) pmt = OTS;
        else if(!strcmp(argv[2],"SOCP")) pmt = SOCP;
        else if(!strcmp(argv[2],"SDP")) pmt = SDP;
        else if(!strcmp(argv[2],"DC")) pmt = DC;
        //else if(!strcmp(argv[2],"QC_OTS")) pmt = QC_OTS;
        else if(!strcmp(argv[2],"SOCP_OTS")) pmt = SOCP_OTS;
            else{
                    cerr << "Unknown model type.\n";
                    exit(1);
            }
        
        if(!strcmp(argv[3],"ipopt")) st = ipopt;
        
        else if(!strcmp(argv[3],"gurobi")) st = gurobi;
        else{
            cerr << "Unknown solver type.\n";
            exit(1);
        }

    }

    cout << "############################## POWERTOOLS ##############################\n\n";
    Net net;
    if(net.readFile(filename)==-1)
        return -1;
//    net.readFile("data/nesta/nesta_case2383wp_mp.m");
//    net.readFile("data/nesta/nesta_case300_ieee.m");
//    net.readFile("../../data/nesta/nesta_case9241_pegase.m");
//    net.readFile("../../data/nesta/nesta_case2383wp_mp.m");
//    net.readFile("../data/nesta/" + filename);
//    PowerModel power_model(pmt,&net);
    cout << "\nTo run PowerTools with a different input/power flow model, enter:\nPowerTools filename ACPOL/ACRECT/SOCP/QC/QC_SDP/SDPDC/OTS/SOCP_OTS ipopt/gurobi\n\n";
    PowerModel power_model(pmt,&net,st);
//    power_model.propagate_bounds();
    power_model.build();
    power_model.min_cost();
    int status = power_model.solve();
    //  Stop timers
    double wall1 = get_wall_time();
    double cpu1  = get_cpu_time();
    cout << "ALL_DATA, " << net._name << ", " << net.nodes.size() << ", " << net.arcs.size() << ", " << power_model._model->_opt<< ", " << status << ", " << wall1 - wall0<< ", -inf\n";
    return 0;

}
