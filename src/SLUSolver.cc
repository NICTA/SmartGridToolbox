#include <SuperLU/SRC/pdsp_defs.h> // superLU_MT

void SLUSolve(int m, int n, int nnz, double * a, int * rowind, int * colptr, int nrhs, double * rhs)
{
   SuperMatrix A, L, U, B;
   int * perm_r; // row permutations from partial pivoting
   int * perm_c; // column permutation vector.
   int info, i, permc_spec;

   // Create matrix A in the format expected by SuperLU.
   dCreate_CompCol_Matrix(&A, m, n, nnz, a, rowind, colptr, SLU_NC, SLU_D, SLU_GE);

   // Create right-hand side matrix B. */
   dCreate_Dense_Matrix(&B, m, nrhs, rhs, m, SLU_DN, SLU_D, SLU_GE);

   if (!(perm_r = intMalloc(m))) abort();
   if (!(perm_c = intMalloc(n))) abort();

   // Populate perm_c
   get_perm_c(1, &A, perm_c);
   // Solve the system
   const int nprocs = 1;
   pdgssv(nprocs, &A, perm_c, perm_r, &L, &B, &B, &info);
}
