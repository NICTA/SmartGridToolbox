#ifndef RANDOM_DOT_H
#define RANDOM_DOT_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>

namespace SmartGridToolbox
{
   /// @addtogroup Common
   /// @{

   /// @name Random number generation.
   /// @{
   
   /// @brief Sample from discrete distribution with nProbs given probabilities.
   double randDiscrete(const double* probs, int nProbs);
   /// @brief Sample from normal distribution with given mean and s.d.
   double randNormal(double mu, double sig);
   /// @brief Sample from uniform distribution with given lower and upper bounds.
   double randUniform(double lower, double upper);
   /// @brief Sample from integer uniform distribution with given lower and upper bounds.
   double randUniformInt(int lower, int upper);
   /// @brief Sample from Weibull distribution with given a and b parameters.
   double randWeibull(double a, double b);
   
   /// @}

   /// @}
}

#endif // RANDOM_DOT_H
