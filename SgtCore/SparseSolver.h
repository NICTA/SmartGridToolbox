#ifndef SPARSE_SOLVER_DOT_H
#define SPARSE_SOLVER_DOT_H

namespace arma
{
    template<typename T> class SpMat;
    template<typename T> class Col;
}

bool kluSolve(const arma::SpMat<double>& a, const arma::Col<double>& b, arma::Col<double>& result);

#endif // SPARSE_SOLVER_DOT_H
