#include <iostream>
#include "Common.h"
#include "SLUSolver.h"

using namespace std;
using namespace SmartGridToolbox;

int main()
{
   ublas::compressed_matrix<double> m(2, 2);
   m(0, 0) = 1.0;
   m(1, 1) = 2.0;
   m.complete_index1_data(); 

   int nnz = m.nnz();

   int * xa = new int[3];
   for (int i = 0; i < 3; ++i) xa[i] = m.index1_data()[i];

   int * asub = new int[nnz];
   for (int i = 0; i < nnz; ++i) asub[i] = m.index2_data()[i];
   
   double * a = new double[nnz];
   for (int i = 0; i < nnz; ++i) a[i] = m.value_data()[i];

   cout << nnz << endl;
   cout << endl;
   for (int i = 0; xa[i] < nnz; ++i) cout << xa[i] << " ";
   cout << endl;
   cout << endl;
   for (int i = 0; i < nnz; ++i) cout << asub[i] << " ";
   cout << endl;
   cout << endl;
   for (int i = 0; i < nnz; ++i) cout << a[i] << " ";
   cout << endl; 
   cout << endl; 

   ublas::vector<double> v(2);
   v(0) = 1;
   v(1) = 5;
   double * b = new double[2];
   for (int i = 0; i < 2; ++i) b[i] = v.data()[i];

   cout << b[0] << " " << b[1] << endl;

   SLUSolve(3, 3, nnz, a, asub, xa, 1, b);
   for (int i = 0; xa[i] < nnz; ++i) cout << xa[i] << " ";
   cout << endl;
   cout << endl;
   for (int i = 0; i < nnz; ++i) cout << asub[i] << " ";
   cout << endl;
   cout << endl;
   for (int i = 0; i < nnz; ++i) cout << a[i] << " ";
   cout << endl; 
   cout << endl; 
   cout << b[0] << " " << b[1] << endl;

   delete[] xa;
   delete[] asub;
   delete[] a;
   delete[] b;
}

