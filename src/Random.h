#ifndef RANDOM_DOT_H
#define RANDOM_DOT_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>

namespace SmartGridToolbox
{
   double randDiscrete(const double * probs, int nProbs);
   double randNormal(double mu, double sig);
   double randUniform(double lower, double upper);
   double randUniformInt(int lower, int upper);
   double randWeibull(double a, double b);
}

#endif // RANDOM_DOT_H
