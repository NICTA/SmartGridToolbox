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

#include <SgtCore/Random.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/weibull_distribution.hpp>

#include <ctime>

namespace Sgt
{
    namespace
    {
        boost::random::mt19937 sRng; // Random number generator.
    }
    
    void randSeedWithTime()
    {
        sRng.seed(static_cast<unsigned int>(std::time(nullptr)));
    }

    void randSeed(const unsigned int n)
    {
        sRng.seed(n);
    }

    double randDiscrete(const double probs[], int nProbs)
    {
        boost::random::discrete_distribution<> dist(&probs[0], &probs[nProbs]);
        return dist(sRng);
    }

    double randNormal(double mu, double sig)
    {
        typedef boost::random::normal_distribution<> DistType;
        static DistType dist(0.0, 1.0);
        return dist(sRng, DistType::param_type(mu, sig));
    }

    double randUniform(double lower, double upper)
    {
        typedef boost::random::uniform_real_distribution<> DistType;
        static DistType dist(0.0, 1.0);
        return dist(sRng,  DistType::param_type(lower, upper));
    }

    double randUniformInt(int lower, int upper)
    {
        typedef boost::random::uniform_int_distribution<> DistType;
        static DistType dist(0, 1);
        return dist(sRng,  DistType::param_type(lower, upper));
    }

    double randWeibull(double a, double b)
    {
        typedef boost::random::weibull_distribution<> DistType;
        static DistType dist(0.0, 1.0);
        return dist(sRng,  DistType::param_type(a, b));
    }
}
