// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#include "PowerFlow.h"

#include <iostream>
#include <sstream>

using namespace arma;

namespace Sgt
{
    std::string to_string(BusType type)
    {
        switch (type)
        {
            case BusType::SL:
                return "SL";
            case BusType::PQ:
                return "PQ";
            case BusType::PV:
                return "PV";
            case BusType::NA:
                return "NA";
            case BusType::BAD:
                return "BAD";
        }
        return "ERROR";
    }

    template<> BusType from_string<BusType>(const std::string& str)
    {
        static BusType busTypes[] = {BusType::SL, BusType::PQ, BusType::PV, BusType::BAD};
        BusType result = BusType::BAD;
        for (BusType* test = &busTypes[0]; *test != BusType::BAD; ++test)
        {
            if (str == to_string(*test))
            {
                result = *test;
            }
        }
        return result;
    }

    std::string to_string(Phase phase)
    {
        switch (phase)
        {
            case Phase::BAL:
                return "BAL";
            case Phase::A:
                return "A";
            case Phase::B:
                return "B";
            case Phase::C:
                return "C";
            case Phase::G:
                return "G";
            case Phase::N:
                return "N";
            case Phase::AB:
                return "AB";
            case Phase::BC:
                return "BC";
            case Phase::CA:
                return "CA";
            case Phase::SY0:
                return "SY0";
            case Phase::SY1:
                return "SY1";
            case Phase::SY2:
                return "SY2";
            case Phase::SPP:
                return "SP";
            case Phase::SPM:
                return "SPM";
            case Phase::SPN:
                return "SPN";
            case Phase::BAD:
                return "BAD";
        }
        return "ERROR";
    }

    template<> Phase from_string<Phase>(const std::string& str)
    {
        const static Phase phases[] = {Phase::BAL, Phase::A, Phase::B, Phase::C, Phase::G, Phase::N, Phase::AB,
            Phase::BC, Phase::CA, Phase::SY0, Phase::SY1, Phase::SY2, Phase::SPP, Phase::SPM, Phase:: SPN,
            Phase::BAD};
        Phase result = Phase::BAD;
        for (const Phase* test = &phases[0]; *test != Phase::BAD; ++test)
        {
            if (str == to_string(*test))
            {
                result = *test;
            }
        }
        return result;
    }

    constexpr uint8_t Phases::noSuchPhase;

    std::string Phases::to_string() const
    {
        std::ostringstream ss;
        ss << "[";
        if (vec_.size() > 0)
        {
            ss << vec_[0];
            for (std::size_t i = 1; i < vec_.size(); ++i)
            {
                ss << ", " << vec_[i];
            }
        }
        ss << "]";
        return ss.str();
    }

    void Phases::buildIndex()
    {
        std::fill_n(index_, gNPhase, noSuchPhase);
        for (uint8_t i = 0; i < vec_.size(); ++i)
        {
            index_[static_cast<uint8_t>(vec_[i])] = i;
        }
    }
    
    template Col<double> mapPhases<Col<double>>(const Col<double>& srcVals,
            const Phases& srcPhases, const Phases& dstPhases);
    template Col<Complex> mapPhases<Col<Complex>>(const Col<Complex>& srcVals,
            const Phases& srcPhases, const Phases& dstPhases);

    template Mat<double> mapPhases<Mat<double>>(const Mat<double>& srcVals,
            const Phases& srcPhases, const Phases& dstPhases);
    template Mat<Complex> mapPhases<Mat<Complex>>(const Mat<Complex>& srcVals,
            const Phases& srcPhases, const Phases& dstPhases);
    
    template arma::Mat<double>& makeUpper<double>(arma::Mat<double>& m);
    template arma::Mat<Complex>& makeUpper<Complex>(arma::Mat<Complex>& m);

    Col<Complex> toDelta(const Col<Complex> V)
    {
        uword nY = V.size();
        sgtAssert(nY <= 3, "toDelta(V): V must have maximum 3 phases.");

        if (nY <= 1)
        {
            return {};
        }
        else if (nY == 2)
        {
            return {V(0) - V(1)};
        }
        else
        {
            return {V(0) - V(1), V(1) - V(2), V(2) - V(0)};
        }
    }

    Mat<Complex> carson(uword nWire, const Mat<double>& Dij, const Col<double> resPerL,
                              double L, double freq, double rhoEarth)
    {
        // Calculate the primative Z matrix (i.e. before Kron)
        Mat<Complex> Z(nWire, nWire, fill::zeros);
        double freqCoeffReal = 9.869611e-7 * freq;
        double freqCoeffImag = 1.256642e-6 * freq;
        double freqAdditiveTerm = 0.5 * log(rhoEarth / freq) + 6.490501;
        for (uword i = 0; i < nWire; ++i)
        {
            Z(i, i) = {resPerL(i) + freqCoeffReal, freqCoeffImag * (log(1 / Dij(i, i)) + freqAdditiveTerm)};
            for (uword k = i + 1; k < nWire; ++k)
            {
                Z(i, k) = {freqCoeffReal, freqCoeffImag * (log(1 / Dij(i, k)) + freqAdditiveTerm)};
                Z(k, i) = Z(i, k);
            }
        }
        Z *= L; // Z has been checked against example in Kersting and found to be OK.

        return Z;
    }

    Mat<Complex> kron(const Mat<Complex>& Z, uword nPhase)
    {
        auto n = Z.n_rows;
        if (n == nPhase)
        {
            return Z;
        }
        else
        {
            sgtAssert(nPhase < n, "Kron reduction: nPhase must be <= the impedance matrix size.");
            auto Zpp = Z.submat(0, 0, nPhase - 1, nPhase - 1);
            auto Zpn = Z.submat(0, nPhase, nPhase - 1, n - 1);
            auto Znp = Z.submat(nPhase, 0, n - 1, nPhase - 1);
            auto Znn = Z.submat(nPhase, nPhase, n - 1, n - 1);
            auto ZnnInv = inv(Znn);

            return (Zpp - Zpn * ZnnInv * Znp);
        }
    }

    Mat<Complex> ZLine2YNode(const Mat<Complex>& ZLine)
    {
        auto n = ZLine.n_rows;

        // The line admittance matrix
        Mat<Complex> YLine = inv(ZLine);

        Mat<Complex>YNode(2 * n, 2 * n, fill::zeros);
        for (uword i = 0; i < n; ++i)
        {
            for (uword j = 0; j < n; ++j)
            {
                YNode(i, j) = YLine(i, j);
                YNode(i, j + n) = -YLine(i, j);
                YNode(i + n, j) = -YLine(i, j);
                YNode(i + n, j + n) = YLine(i, j);
            }
        }

        return YNode;
    }

    double bundleGmr(unsigned int n, double gmr1, double d)
    {
        double result;
        if (n == 1)
        {
            return gmr1;
        }
        else
        {
            double rConc = d / (2 * std::sin(pi / n));
            result = std::pow(gmr1 * n * std::pow(rConc, n - 1), 1.0 / n);
        }
        return result;
    }

    namespace
    {
        const Complex alpha{-0.5, 0.866025403784439};
        const Complex alpha2{-0.5, -0.866025403784439};
        const double oneThird = 1.0 / 3.0;
        const double sqrt3 = std::sqrt(3.0);
        const double sqrt3Inv = 1.0 / sqrt3;

        const Mat<Complex>::fixed<3, 3> A{{1, 1, 1}, {1, alpha2, alpha}, {1, alpha, alpha2}};
        const Mat<Complex>::fixed<3, 3> AInv(oneThird * 
                Mat<Complex>::fixed<3, 3>{{1, 1, 1}, {1, alpha, alpha2}, {1, alpha2, alpha}});

        const Mat<Complex>::fixed<3, 3> B(sqrt3Inv * A);
        const Mat<Complex>::fixed<3, 3> BInv(sqrt3 * AInv);
    }

    Col<Complex> toSequence(const Col<Complex>& v)
    {
        return AInv * v;
    }

    Mat<Complex> toSequence(const Mat<Complex>& M)
    {
        return AInv * M * A;
    }

    Col<Complex> fromSequence(const Col<Complex>& v)
    {
        return A * v;
    }

    Mat<Complex> fromSequence(const Mat<Complex>& M)
    {
        return A * M * AInv;
    }
 
    Col<Complex> toScaledSequence(const Col<Complex>& v)
    {
        return BInv * v;
    }

    Mat<Complex> toScaledSequence(const Mat<Complex>& M)
    {
        return BInv * M * B;
    }

    Col<Complex> fromScaledSequence(const Col<Complex>& v)
    {
        return B * v;
    }

    Mat<Complex> fromScaledSequence(const Mat<Complex>& M)
    {
        return B * M * BInv;
    }
      
    Mat<Complex> approxPhaseImpedanceMatrix(Complex ZPlus, Complex Z0)
    {
        return oneThird * Mat<Complex>{{2 * ZPlus + Z0, Z0 - ZPlus,    Z0 - ZPlus}, 
                                       {Z0 - ZPlus,     2  * ZPlus + Z0, Z0 - ZPlus},
                                       {Z0 - ZPlus,     Z0 - ZPlus,    2 * ZPlus + Z0}};
    }
}
