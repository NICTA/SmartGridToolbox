#include <iostream>
#include "BalancedPowerFlowNR.h"
#include "Common.h"

using namespace std;
using namespace SmartGridToolbox;

int main()
{
   BalancedPowerFlowNR bpf;
   bpf.addBus(0, BusType::SL, 1.0, 0.0, 0.0, 0.0);
   bpf.addBus(1, BusType::PQ, {1.07, 0.04}, 0.0, 0.0, {0.15, 0.01});
   bpf.addBranch(0, 1, Array2D<Complex, 2, 2>{{{0.1, -0.1},{-0.1, 0.1}}});
   bpf.validate();
   bpf.solve();
}
