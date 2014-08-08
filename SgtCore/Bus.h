#ifndef BUS_DOT_H
#define BUS_DOT_H

#include <LibPowerFlow/Gen.h>
#include <LibPowerFlow/PowerFlow.h>
#include <LibPowerFlow/Zip.h>

#include<iostream>
#include<map>
#include<vector>

namespace LibPowerFlow
{
   namespace ublas = boost::numeric::ublas;

   /// @brief A Bus is a grouped set of conductors / terminals, one per phase. 
   class Bus
   {
      friend class Network;
   
      public:
         
         typedef std::vector<Zip*> ZipVec;
         typedef std::vector<Gen*> GenVec;

      /// @name Lifecycle:
      /// @{

         Bus(const std::string& id, Phases phases) :
            id_(id), phases_(phases), VNom_(phases.size(), czero), V_(phases.size(), czero)
         {
            // Empty.
         }

         virtual ~Bus()
         {
            // Empty.
         }

      /// @}

      /// @name Basic identity and type:
      /// @{
         
         const std::string& id() const
         {
            return id_;
         }
         
         void setId(const std::string& id)
         {
            id_ = id;
         }
         
         const Phases& phases() const
         {
            return phases_;
         }
         
         void setPhases(const Phases& phases) 
         {
            phases_ = phases;
         }
         
         BusType type() const
         {
            return type_;
         }
         
         void setType(const BusType type) 
         {
            type_ = type;
         }
         
         double VBase() const
         {
            return VBase_;
         }
         
         void setVBase(double VBase)
         {
            VBase_ = VBase;
         }
         
         ublas::vector<Complex> VNom() const
         {
            return VNom_;
         }
         
         void setVNom(const ublas::vector<Complex>& VNom)
         {
            VNom_ = VNom;
         }

      /// @}
         
      /// @name ZIPs
      /// @{
         
         const ZipVec& zips() const
         {
            return zips_;
         }

      /// @}
      
      /// @name Generation
      /// @{
         
         const GenVec& gens() const
         {
            return gens_;
         }
         
      /// @}
      
      /// @name Voltage magnitude limits
      /// @{

         double VMagMin() const
         {
            return VMagMin_;
         }

         void setVMagMin(double VMagMin)
         {
            VMagMin_ = VMagMin;
         }
  
         double VMagMax() const
         {
            return VMagMax_;
         }

         void setVMagMax(double VMagMax)
         {
            VMagMax_ = VMagMax;
         }
                              
      /// @}
         
      /// @name In service 
      /// @{
         
         bool isInService()
         {
            return isInService_;
         }

         void setIsInService(bool isInService)
         {
            isInService_ = isInService;
         }
      
      /// @}
                              
      /// @name State
      /// @{
         
         const ublas::vector<Complex>& V() const {return V_;}

         void setV(const ublas::vector<Complex>& V) {V_ = V;}

      /// @}

      /// @name Output
      /// @{
         
         friend std::ostream& operator<<(std::ostream& os, const Bus& bus)
         {
            return bus.print(os);
         }
      
      /// @}
      
      protected:
         
         void addZip(Zip& zip)
         {
            zips_.push_back(&zip);
         }

         void addGen(Gen& gen)
         {
            gens_.push_back(&gen);
         }
         
         virtual std::ostream& print(std::ostream& os) const;
      
      private:

         std::string id_;
         Phases phases_;

         BusType type_{BusType::NA};
         double VBase_{1.0};
         ublas::vector<Complex> VNom_;
         
         ZipVec zips_;
         GenVec gens_;
         
         double VMagMin_{-infinity};
         double VMagMax_{infinity};

         bool isInService_{true};
         ublas::vector<Complex> V_;
   };
}

#endif // BUS_DOT_H
