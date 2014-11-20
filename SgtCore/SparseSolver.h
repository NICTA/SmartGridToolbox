#ifndef SPARSE_SOLVER_DOT_H
#define SPARSE_SOLVER_DOT_H

bool kluSolve(const boost::numeric::arma::SpMat<double>& a,
              const boost::numeric::arma::Col<double>& b,
              boost::numeric::arma::Col<double>& result);

#endif // SPARSE_SOLVER_DOT_H
