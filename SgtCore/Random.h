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

#ifndef RANDOM_DOT_H
#define RANDOM_DOT_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>

namespace Sgt
{
    /// @addtogroup Utilities
    /// @{

    /// @name Random number generation.
    /// @{

    /// @brief Seed random generator with unsigned int.
    /// @ingroup Utilities
    void randSeed(const unsigned int n);
    /// @brief Seed random generator with time.
    /// @ingroup Utilities
    void randSeedWithTime();
    /// @brief Sample from discrete distribution with nProbs given probabilities.
    /// @ingroup Utilities
    double randDiscrete(const double probs[], int nProbs);
    /// @brief Sample from normal distribution with given mean and s.d.
    /// @ingroup Utilities
    double randNormal(double mu, double sig);
    /// @brief Sample from uniform distribution with given lower and upper bounds.
    /// @ingroup Utilities
    double randUniform(double lower, double upper);
    /// @brief Sample from integer uniform distribution with given lower and upper bounds.
    /// @ingroup Utilities
    int randUniformInt(int lower, int upper);
    /// @brief Sample from Weibull distribution with given a and b parameters.
    /// @ingroup Utilities
    double randWeibull(double a, double b);

    /// @}

    /// @}
}

#endif // RANDOM_DOT_H
