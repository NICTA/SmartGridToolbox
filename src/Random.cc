#include <boost/random.hpp>

namespace SmartGridToolbox 
{
   static boost::random::mt19937 sRng; // Random number generator.

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

   double RandWeibull(double a, double b)
   {
      typedef boost::random::weibull_distribution<> DistType;
      static DistType dist(0.0, 1.0); 
      return dist(sRng,  DistType::param_type(a, b));
   }

   double RandUniformInt(int lower, int upper)
   {
      typedef boost::random::weibull_distribution<> DistType;
      static DistType dist(0, 1); 
      return dist(sRng,  DistType::param_type(lower, upper));
   }
}
