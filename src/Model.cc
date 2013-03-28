#include "Model.h"
#include "Component.h"
#include "Output.h"

namespace SmartGridToolbox 
{
   Model::~Model() 
   {
   }

   void Model::AddComponent(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = 
         compMap_.insert(make_pair(comp.GetName(), &comp));
      if (result.second == 0) {
         Error("Component already exists in model!");
      }
      else
      {
         compVec_.push_back(&comp);
      }
   }

   double Model::RandNormal(double mu, double sig)
   {
      typedef boost::random::normal_distribution<> DistType;
      static DistType dist(0.0, 1.0); 
      return dist(rng_, DistType::param_type(mu, sig));
   }
   
   double Model::RandUniform(double lower, double upper)
   {
      typedef boost::random::uniform_real_distribution<> DistType;
      static DistType dist(0.0, 1.0); 
      return dist(rng_,  DistType::param_type(lower, upper));
   }
}
