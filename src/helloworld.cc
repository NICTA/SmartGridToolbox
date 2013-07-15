#include <iostream>
#include "Common.h"
#include "SparseSolver.h"

using namespace std;
//using namespace SmartGridToolbox;

int main()
{
   int n = 5;
   boost::numeric::ublas::compressed_matrix<double> a(n, n);
   a(0, 0) = 2.0;
   a(0, 1) = 3.0;
   a(1, 0) = 3.0;
   a(1, 2) = 4.0;
   a(1, 4) = 6.0;
   a(2, 1) = -1.0;
   a(2, 2) = -3.0;
   a(2, 3) = 2.0;
   a(3, 2) = 1.0;
   a(4, 1) = 4.0;
   a(4, 2) = 2.0;
   a(4, 4) = 1.0;

   boost::numeric::ublas::vector<double> b(n);
   b(0) = 8.0;
   b(1) = 45.0;
   b(2) = -3.0;
   b(3) = 3.0;
   b(4) = 19.0;

   boost::numeric::ublas::vector<double> x(n);
   KLUSolve(a, b, x);
   for (int i = 0; i < n; ++i) cout << x(i) << " ";
   cout << endl;
}

