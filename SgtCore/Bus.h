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
   class Bus : public Component
   {
      friend class Network;

      public:

      /// @name Lifecycle:
      /// @{

         Bus(const std::string& id, Phases phases, const ublas::vector<Complex>& VNom, double VBase);

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const {return "bus";}

      /// @}

      /// @name Basic identity and type:
      /// @{

         virtual const Phases& phases() const
         {
            return phases_;
         }

         virtual void setPhases(const Phases& phases)
         {
            phases_ = phases;
         }

         virtual ublas::vector<Complex> VNom() const
         {
            return VNom_;
         }

         virtual void setVNom(const ublas::vector<Complex>& VNom)
         {
            VNom_ = VNom;
            controlChanged_.trigger();
         }

         virtual double VBase() const
         {
            return VBase_;
         }

         virtual void setVBase(double VBase)
         {
            VBase_ = VBase;
         }

      /// @}

      /// @name Control and limits:
      /// @{

         virtual BusType type() const
         {
            return type_;
         }

         virtual void setType(const BusType type)
         {
            type_ = type;
            controlChanged_.trigger();
         }

         virtual ublas::vector<double> VMagSetpoint() const
         {
            return VMagSetpoint_;
         }

         virtual void setVMagSetpoint(const ublas::vector<double>& VMagSetpoint)
         {
            VMagSetpoint_ = VMagSetpoint;
            controlChanged_.trigger();
         }

         virtual ublas::vector<double> VAngSetpoint() const
         {
            return VAngSetpoint_;
         }

         virtual void setVAngSetpoint(const ublas::vector<double>& VAngSetpoint)
         {
            VAngSetpoint_ = VAngSetpoint;
            controlChanged_.trigger();
         }

         virtual double VMagMin() const
         {
            return VMagMin_;
         }

         virtual void setVMagMin(double VMagMin)
         {
            VMagMin_ = VMagMin;
            controlChanged_.trigger();
         }

         virtual double VMagMax() const
         {
            return VMagMax_;
         }

         virtual void setVMagMax(double VMagMax)
         {
            VMagMax_ = VMagMax;
            controlChanged_.trigger();
         }

      /// @}

      /// @name State
      /// @{

         virtual bool isInService()
         {
            return isInService_;
         }

         virtual void setIsInService(bool isInService)
         {
            isInService_ = isInService;
            isInServiceChanged_.trigger();
         }

         virtual const ublas::vector<Complex>& V() const {return V_;}

         virtual void setV(const ublas::vector<Complex>& V) {V_ = V;}

      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() {return isInServiceChanged_;}

         /// @brief Event triggered when bus control (e.g. setpoint) has changed.
         virtual Event& controlChanged() {return controlChanged_;}
         
         /// @brief Event triggered when bus state (e.g. voltage) has been updated.
         virtual Event& voltageUpdated() {return voltageUpdated_;}
      
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
