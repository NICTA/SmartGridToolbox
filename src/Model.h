#ifndef MODEL_DOT_H
#define MODEL_DOT_H

#include <map>
#include <string>
using std::string;
namespace SmartGridToolbox
{
   class Component;

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

         void AddComponent(Component & comp);

         template<typename T> const T * GetComponent(const std::string & name)
            const
         {
            ComponentMapType::const_iterator it = compMap_.find(name);
            return (it == compMap_.end()) 
               ? 0 : dynamic_cast<const T *>(it->second);
         }

         template<typename T> T * GetComponent(const std::string & name)
         {
            return const_cast<T *>((const_cast<const Model *>(this))->
                  GetComponent<T>(name));
         }

      private:
         typedef std::pair<std::string, Component *> ComponentPairType;
         typedef std::map<std::string, Component *> ComponentMapType;

         ComponentMapType compMap_;
   };
}

#endif // MODEL_DOT_H
