#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/weibull_distribution.hpp>

namespace SmartGridToolbox 
{
   static boost::random::mt19937 sRng; // Random number generator.

   double RandDiscrete(const double * probs, int nProbs)
   {
      boost::random::discrete_distribution<> dist(&probs[0], &probs[nProbs]);
      return dist(sRng);
   }

   double RandNormal(double mu, double sig)
   {
      typedef boost::random::normal_distribution<> DistType;
      static DistType dist(0.0, 1.0); 
      return dist(sRng, DistType::param_type(mu, sig));
   }
   
   double RandUniform(double lower, double upper)
   {
      typedef boost::random::uniform_real_distribution<> DistType;
      static DistType dist(0.0, 1.0); 
      return dist(sRng,  DistType::param_type(lower, upper));
   }

   double RandUniformInt(int lower, int upper)
   {
      typedef boost::random::uniform_int_distribution<> DistType;
      static DistType dist(0, 1); 
      return dist(sRng,  DistType::param_type(lower, upper));
   }

   double RandWeibull(double a, double b)
   {
      typedef boost::random::weibull_distribution<> DistType;
      static DistType dist(0.0, 1.0); 
      return dist(sRng,  DistType::param_type(a, b));
   }
}
