#ifndef COMPONENT_TEMPLATE_DOT_H
#define COMPONENT_TEMPLATE_DOT_H

#include <iostream>

namespace Sgt
{
    class ComponentTemplate : public Component
    {
        /// @name Overridden member functions from SimComponent.
        /// @{

        public:
            virtual Time validUntil() const override;

        protected:
            virtual void initializeState() override;
            virtual void updateState(Time t) override;

        /// @}

        /// @name My member functions.
        /// @{

        public:
            ComponentTemplate(const std::string& name);
            virtual ~ComponentTemplate();

        private:

        /// @}

        private:
    }
}

#endif // COMPONENT_TEMPLATE_DOT_H
