// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SPARSE_HELPER_DOT_H
#define SPARSE_HELPER_DOT_H

#include <armadillo>

namespace Sgt
{
    /// @brief Object to efficiently construct large sparse matrices in an incremental manner.
    ///
    /// Use insert(...) to add elements, and when ready, get() to construct and return the sparse matrix.
    /// @ingroup Utilities
    template<typename T> class SparseHelper
    {
        public:

        SparseHelper(arma::uword nRow, arma::uword nCol, bool sort = true, bool add = true, 
                bool checkZeros = false, unsigned int nBatch = (1 << 16)) :
            locs_(2, nBatch, arma::fill::none),
            vals_(nBatch, arma::fill::none),
            sort_(sort),
            add_(add),
            checkZeros_(checkZeros),
            m_(nRow, nCol)
        {
            // Empty.
        }

        void insert(arma::uword iRow, arma::uword iCol, const T& x)
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
                m_ += arma::SpMat<T>(add_, locs_(sp1, sp2), vals_(sp2), m_.n_rows, m_.n_cols, sort_, checkZeros_);
                nWaiting_ = 0;
            }
        }


        private:

        arma::uword nWaiting_{0};
        arma::Mat<arma::uword> locs_;
        arma::Col<T> vals_;
        bool sort_;
        bool add_;
        bool checkZeros_;
        arma::SpMat<T> m_;
    };
}

#endif // SPARSE_HELPER_DOT_H
