#ifndef SPARSE_SOLVER_DOT_H
#define SPARSE_SOLVER_DOT_H

bool kluSolve(const boost::numeric::ublas::compressed_matrix<double>& a,
              const boost::numeric::ublas::vector<double>& b,
              boost::numeric::ublas::vector<double>& result);

#endif // SPARSE_SOLVER_DOT_H
