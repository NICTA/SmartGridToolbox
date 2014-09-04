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

      /// @name Basic identity and type:
      /// @{

         virtual const Phases& phases() const = 0;
         virtual ublas::vector<Complex> VNom() const = 0;
         virtual double VBase() const = 0;

      /// @}

      /// @name Control and limits:
      /// @{

         virtual BusType type() const = 0;
         virtual void setType(const BusType type) = 0;

         virtual ublas::vector<double> VMagSetpoint() const = 0;
         virtual void setVMagSetpoint(const ublas::vector<double>& VMagSetpoint) = 0;

         virtual ublas::vector<double> VAngSetpoint() const = 0;
         virtual void setVAngSetpoint(const ublas::vector<double>& VAngSetpoint) = 0;

         virtual double VMagMin() const = 0;
         virtual void setVMagMin(double VMagMin) = 0;

         virtual double VMagMax() const = 0;
         virtual void setVMagMax(double VMagMax) = 0;

      /// @}

      /// @name State
      /// @{

         virtual bool isInService() = 0;
         virtual void setIsInService(bool isInService) = 0;

         virtual const ublas::vector<Complex>& V() const = 0;
         virtual void setV(const ublas::vector<Complex>& V) = 0;

      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() = 0;

         /// @brief Event triggered when bus control (e.g. setpoint) has changed.
         virtual Event& controlChanged() = 0;
         
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

         Bus(const std::string& id, Phases phases, const ublas::vector<Complex>& VNom, double VBase);

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const override
         {
            return "bus";
         }

      /// @}

      /// @name Basic identity and type:
      /// @{

         virtual const Phases& phases() const override
         {
            return phases_;
         }

         virtual ublas::vector<Complex> VNom() const override
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
            controlChanged_.trigger();
         }

         virtual ublas::vector<double> VMagSetpoint() const override
         {
            return VMagSetpoint_;
         }

         virtual void setVMagSetpoint(const ublas::vector<double>& VMagSetpoint) override
         {
            VMagSetpoint_ = VMagSetpoint;
            controlChanged_.trigger();
         }

         virtual ublas::vector<double> VAngSetpoint() const override
         {
            return VAngSetpoint_;
         }

         virtual void setVAngSetpoint(const ublas::vector<double>& VAngSetpoint) override
         {
            VAngSetpoint_ = VAngSetpoint;
            controlChanged_.trigger();
         }

         virtual double VMagMin() const override
         {
            return VMagMin_;
         }

         virtual void setVMagMin(double VMagMin) override
         {
            VMagMin_ = VMagMin;
            controlChanged_.trigger();
         }

         virtual double VMagMax() const override
         {
            return VMagMax_;
         }

         virtual void setVMagMax(double VMagMax) override
         {
            VMagMax_ = VMagMax;
            controlChanged_.trigger();
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

         virtual const ublas::vector<Complex>& V() const override
         {
            return V_;
         }

         virtual void setV(const ublas::vector<Complex>& V) override
         {
            V_ = V;
         }

      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() override
         {
            return isInServiceChanged_;
         }

         /// @brief Event triggered when bus control (e.g. setpoint) has changed.
         virtual Event& controlChanged() override
         {
            return controlChanged_;
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
         ublas::vector<Complex> VNom_{phases_.size()};
         double VBase_{1.0};

         BusType type_{BusType::NA};
         ublas::vector<double> VMagSetpoint_{phases_.size()};
         ublas::vector<double> VAngSetpoint_{phases_.size()};
         double VMagMin_{-infinity};
         double VMagMax_{infinity};

         bool isInService_{true};
         ublas::vector<Complex> V_{phases_.size(), czero};

         Event isInServiceChanged_;
         Event controlChanged_;
         Event voltageUpdated_;
   };
}

#endif // BUS_DOT_H
