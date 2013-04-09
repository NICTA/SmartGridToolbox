#ifndef RANDOM_DOT_H
#define RANDOM_DOT_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>

namespace SmartGridToolbox
{
   double RandDiscrete(const double * probs, int nProbs);
   double RandNormal(double mu, double sig);
   double RandUniform(double lower, double upper);
   double RandUniformInt(int lower, int upper);
   double RandWeibull(double a, double b);
}

#endif // RANDOM_DOT_H
