#include <iostream>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include "SparseSolver.h"

int main()
{
   std::cout << "Hello, world." << std::endl;
   using namespace boost::numeric::ublas;

   int n = 5;
   compressed_matrix<double> a(n, n);
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

   vector<double> b(n);
   b(0) = 8.0;
   b(1) = 45.0;
   b(2) = -3.0;
   b(3) = 3.0;
   b(4) = 19.0;

   vector<double> x(n);

   KLUSolve(a, b, x);
}
