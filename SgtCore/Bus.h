#ifndef BUS_DOT_H
#define BUS_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include<iostream>
#include<map>
#include<vector>

namespace SmartGridToolbox
{
   /// @brief A Bus is a grouped set of conductors / terminals, one per phase.
   class BusInterface : virtual public ComponentInterface
   {
      public:

      /// @name Lifecycle:
      /// @{

         virtual ~BusInterface() = default;

      /// @}
         
      /// @name Component Type:
      /// @{
         
         static constexpr const char* sComponentType()
         {
            return "bus";
         }

      /// @}
     
      /// @name Basic identity and type:
      /// @{

         virtual const Phases& phases() const = 0;
         virtual arma::Col<Complex> VNom() const = 0;
         virtual double VBase() const = 0;

      /// @}

      /// @name Control and limits:
      /// @{

         virtual BusType type() const = 0;
         virtual void setType(const BusType type) = 0;

         virtual arma::Col<double> VMagSetpoint() const = 0;
         virtual void setVMagSetpoint(const arma::Col<double>& VMagSetpoint) = 0;

         virtual arma::Col<double> VAngSetpoint() const = 0;
         virtual void setVAngSetpoint(const arma::Col<double>& VAngSetpoint) = 0;

         virtual void applyVSetpoints() = 0;

         virtual double VMagMin() const = 0;
         virtual void setVMagMin(double VMagMin) = 0;

         virtual double VMagMax() const = 0;
         virtual void setVMagMax(double VMagMax) = 0;

      /// @}

      /// @name State
      /// @{

         virtual bool isInService() = 0;
         virtual void setIsInService(bool isInService) = 0;

         virtual const arma::Col<Complex>& V() const = 0;
         virtual void setV(const arma::Col<Complex>& V) = 0;

      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() = 0;

         /// @brief Event triggered when bus setpoint has changed.
         virtual Event& setpointChanged() = 0;
         
         /// @brief Event triggered when bus state (e.g. voltage) has been updated.
         virtual Event& voltageUpdated() = 0;
      
      /// @}
   };

   /// @brief A Bus is a grouped set of conductors / terminals, one per phase.
   class Bus : public Component, virtual public BusInterface
   {
      friend class Network;

      public:

      /// @name Lifecycle:
      /// @{

         Bus(const std::string& id, const Phases& phases, const arma::Col<Complex>& VNom, double VBase);

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      /// @}

      /// @name Basic identity and type:
      /// @{

         virtual const Phases& phases() const override
         {
            return phases_;
         }
         SGT_PROP_GET(phases, Bus, Phases, GetByConstRef, phases);

         virtual arma::Col<Complex> VNom() const override
         {
            return VNom_;
         }

         virtual double VBase() const override
         {
            return VBase_;
         }

      /// @}

      /// @name Control and limits:
      /// @{

         virtual BusType type() const override
         {
            return type_;
         }

         virtual void setType(const BusType type) override
         {
            type_ = type;
            setpointChanged_.trigger();
         }

         virtual arma::Col<double> VMagSetpoint() const override
         {
            return VMagSetpoint_;
         }

         virtual void setVMagSetpoint(const arma::Col<double>& VMagSetpoint) override
         {
            VMagSetpoint_ = VMagSetpoint;
            setpointChanged_.trigger();
         }
         
         virtual arma::Col<double> VAngSetpoint() const override
         {
            return VAngSetpoint_;
         }

         virtual void setVAngSetpoint(const arma::Col<double>& VAngSetpoint) override
         {
            VAngSetpoint_ = VAngSetpoint;
            setpointChanged_.trigger();
         }

         virtual void applyVSetpoints();

         virtual double VMagMin() const override
         {
            return VMagMin_;
         }

         virtual void setVMagMin(double VMagMin) override
         {
            VMagMin_ = VMagMin;
            setpointChanged_.trigger();
         }

         virtual double VMagMax() const override
         {
            return VMagMax_;
         }

         virtual void setVMagMax(double VMagMax) override
         {
            VMagMax_ = VMagMax;
            setpointChanged_.trigger();
         }

      /// @}

      /// @name State
      /// @{

         virtual bool isInService() override
         {
            return isInService_;
         }

         virtual void setIsInService(bool isInService) override
         {
            isInService_ = isInService;
            isInServiceChanged_.trigger();
         }

         virtual const arma::Col<Complex>& V() const override
         {
            return V_;
         }

         virtual void setV(const arma::Col<Complex>& V) override
         {
            V_ = V;
            voltageUpdated_.trigger();
         }

      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() override
         {
            return isInServiceChanged_;
         }

         /// @brief Event triggered when bus setpoint has changed.
         virtual Event& setpointChanged() override
         {
            return setpointChanged_;
         }
         
         /// @brief Event triggered when bus state (e.g. voltage) has been updated.
         virtual Event& voltageUpdated() override
         {
            return voltageUpdated_;
         }
      
      /// @}
      
      /// @name Printing.
      /// @{
         
         virtual void print(std::ostream& os) const override;
      
      /// @}

      private:

         Phases phases_{Phase::BAL};
         arma::Col<Complex> VNom_;
         double VBase_{1.0};

         BusType type_{BusType::NA};
         arma::Col<double> VMagSetpoint_;
         arma::Col<double> VAngSetpoint_;
         double VMagMin_{-infinity};
         double VMagMax_{infinity};

         bool isInService_{true};
         arma::Col<Complex> V_;

         Event isInServiceChanged_{std::string(sComponentType()) + " : Is in service changed"};
         Event setpointChanged_{std::string(sComponentType()) + " : Setpoint changed"};
         Event voltageUpdated_{std::string(sComponentType()) + " : Voltage updated"};
   };
}

#endif // BUS_DOT_H
