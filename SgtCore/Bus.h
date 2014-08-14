#ifndef BUS_DOT_H
#define BUS_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/ComponentAbc.h>
#include <SgtCore/Gen.h>
#include <SgtCore/PowerFlow.h>
#include <SgtCore/Zip.h>

#include<iostream>
#include<map>
#include<vector>

namespace SmartGridToolbox
{
   /// @brief A Bus is a grouped set of conductors / terminals, one per phase.
   class Bus : public ComponentAbc
   {
      friend class Network;

      public:

         typedef std::vector<Zip*> ZipVec;
         typedef std::vector<Gen*> GenVec;

      /// @name Lifecycle:
      /// @{

         Bus(const std::string& id, Phases phases, const ublas::vector<Complex>& VNom, double VBase);

         virtual ~Bus() = default;

      /// @}

      /// @name Basic identity and type:
      /// @{

         virtual const std::string& id() const
         {
            return id_;
         }

         virtual void setId(const std::string& id)
         {
            id_ = id;
         }

         const Phases& phases() const
         {
            return phases_;
         }

         virtual void setPhases(const Phases& phases)
         {
            phases_ = phases;
         }

         ublas::vector<Complex> VNom() const
         {
            return VNom_;
         }

         virtual void setVNom(const ublas::vector<Complex>& VNom)
         {
            VNom_ = VNom;
         }

         double VBase() const
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

         BusType type() const
         {
            return type_;
         }

         virtual void setType(const BusType type)
         {
            type_ = type;
         }

         ublas::vector<double> VMagSetpoint() const
         {
            return VMagSetpoint_;
         }

         virtual void setVMagSetpoint(const ublas::vector<double>& VMagSetpoint)
         {
            VMagSetpoint_ = VMagSetpoint;
         }

         ublas::vector<double> VAngSetpoint() const
         {
            return VAngSetpoint_;
         }

         virtual void setVAngSetpoint(const ublas::vector<double>& VAngSetpoint)
         {
            VAngSetpoint_ = VAngSetpoint;
         }

         double VMagMin() const
         {
            return VMagMin_;
         }

         virtual void setVMagMin(double VMagMin)
         {
            VMagMin_ = VMagMin;
         }

         double VMagMax() const
         {
            return VMagMax_;
         }

         virtual void setVMagMax(double VMagMax)
         {
            VMagMax_ = VMagMax;
         }

      /// @}


      /// @name ZIPs
      /// @{

         const ZipVec& zips() const
         {
            return zips_;
         }

         /// @brief Total ZIP admittance.
         ublas::vector<Complex> YZip() const;

         /// @brief Total ZIP current injection into bus.
         ublas::vector<Complex> IZip() const;

         /// @brief Total ZIP power injection into bus.
         ublas::vector<Complex> SZip() const;

      /// @}

      /// @name Generation
      /// @{
         
         const GenVec& gens() const
         {
            return gens_;
         }
         
         /// @brief Total generated power injection (-ve of generated power).
         ublas::vector<Complex> SGen() const;

      /// @}

      /// @name State
      /// @{

         bool isInService()
         {
            return isInService_;
         }

         virtual void setIsInService(bool isInService)
         {
            isInService_ = isInService;
         }

         const ublas::vector<Complex>& V() const {return V_;}

         virtual void setV(const ublas::vector<Complex>& V) {V_ = V;}

      /// @}

      /// @name Output
      /// @{

         friend std::ostream& operator<<(std::ostream& os, const Bus& bus)
         {
            return bus.print(os);
         }

      /// @}

      protected:

         virtual void addZip(Zip& zip)
         {
            zips_.push_back(&zip);
         }

         virtual void addGen(Gen& gen)
         {
            gens_.push_back(&gen);
         }

         virtual std::ostream& print(std::ostream& os) const;

      private:

         std::string id_{"UNDEFINED"};
         Phases phases_{Phase::BAL};
         ublas::vector<Complex> VNom_{phases_.size()};
         double VBase_{1.0};

         BusType type_{BusType::NA};
         ublas::vector<double> VMagSetpoint_{phases_.size()};
         ublas::vector<double> VAngSetpoint_{phases_.size()};
         double VMagMin_{-infinity};
         double VMagMax_{infinity};

         ZipVec zips_;
         GenVec gens_;

         bool isInService_{true};
         ublas::vector<Complex> V_{phases_.size(), czero};
   };
}

#endif // BUS_DOT_H
