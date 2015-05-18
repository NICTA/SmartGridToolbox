#ifndef SPARSE_HELPER_DOT_H
#define SPARSE_HELPER_DOT_H

#include <armadillo>

namespace Sgt
{
    template<typename T> class SparseHelper
    {
        public:

            SparseHelper(int nRow, int nCol, bool sort = true, bool add = true, bool checkZeros = false, 
                         int nBatch = (1 << 16)) :
                locs_(2, nBatch, arma::fill::none),
                vals_(nBatch, arma::fill::none),
                sort_(sort),
                add_(add),
                checkZeros_(checkZeros),
                m_(nRow, nCol)
            {
                // Empty.
            }

            void insert(int iRow, int iCol, const T& x)
            {
                if (nWaiting_ == vals_.size())
                {
                    flush();
                }

                locs_(0, nWaiting_) = iRow;
                locs_(1, nWaiting_) = iCol;
                vals_(nWaiting_) = x;
                ++nWaiting_;
            }
            
            arma::SpMat<T> get()
            {
                flush();
                return std::move(m_); // Should apply move constructor.
            }

        private:

            void flush()
            {
                if (nWaiting_ > 0)
                {
                    arma::span sp1(0, 1);
                    arma::span sp2(0, nWaiting_ - 1);
                    m_ += arma::SpMat<T>(checkZeros_, locs_(sp1, sp2), vals_(sp2), m_.n_rows, m_.n_cols, sort_, add_);
                    nWaiting_ = 0;
                }
            }


        private:

            int nWaiting_{0};
            arma::Mat<arma::uword> locs_;
            arma::Col<T> vals_;
            bool sort_;
            bool add_;
            bool checkZeros_;
            arma::SpMat<T> m_;
    };
}

#endif // SPARSE_HELPER_DOT_H
