#ifndef MODEL_DOT_H
#define MODEL_DOT_H

#include <map>
#include <string>
using std::string;
namespace MGSim
{
   class Object;

   /// Simulation environment, containing global simulation data.
   /// Units: All units are internally stored in SI.
   class Model
   {
      public:
         /// Default constructor.
         Model()
         {
            // Empty.
         }

         /// Destructor.
         ~Model();

         void AddObject(Object * obj);

         template<typename T> const T * GetObject(const std::string & name) const
         {
            ObjectMapType::iterator it = objectMap_.find(name);
            return (it == objectMap_.end()) ? 0 : dynamic_cast<T*>(*it);
         }

         template<typename T> T * GetObject(const std::string & name)
         {
            return const_cast<T *>((const_cast<const Model *>(this))->
                  GetObject<T>(name));
         }

      private:
         typedef std::pair<std::string, Object *> ObjectPairType;
         typedef std::map<std::string, Object *> ObjectMapType;

         ObjectMapType objectMap_;
   };
}

#endif // MODEL_DOT_H
