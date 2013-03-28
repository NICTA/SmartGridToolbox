#ifndef RANDOM_DOT_H
#define RANDOM_DOT_H

namespace SmartGridToolbox
{
   double RandNormal(double mu, double sig);
   double RandUniform(double lower, double upper);
   double RandUniformInt(int lower, int upper);
   double RandWeibull(double a, double b);
}

#endif // RANDOM_DOT_H
