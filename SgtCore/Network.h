#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include <SgtCore/Branch.h>
#include <SgtCore/Bus.h>
#include <SgtCore/Component.h>
#include <SgtCore/Gen.h>
#include <SgtCore/PowerFlow.h>
#include <SgtCore/Zip.h>

#include<iostream>
#include<map>
#include<memory>
#include<vector>

namespace SmartGridToolbox
{
   class Arc;
   class Node;

   typedef std::shared_ptr<const BranchAbc> ConstBranchPtr;
   typedef std::shared_ptr<BranchAbc> BranchPtr;
   
   typedef std::shared_ptr<const BusInterface> ConstBusPtr;
   typedef std::shared_ptr<BusInterface> BusPtr;

   typedef std::shared_ptr<const GenInterface> ConstGenPtr;
   typedef std::shared_ptr<GenInterface> GenPtr;

   typedef std::shared_ptr<const ZipInterface> ConstZipPtr;
   typedef std::shared_ptr<ZipInterface> ZipPtr;
   
   typedef std::shared_ptr<const Node> ConstNodePtr;
   typedef std::shared_ptr<Node> NodePtr;
   
   typedef std::shared_ptr<const Arc> ConstArcPtr;
   typedef std::shared_ptr<Arc> ArcPtr;
   
   typedef std::vector<BusPtr> BusVec;
   typedef std::map<std::string, BusPtr> BusMap;

   typedef std::vector<BranchPtr> BranchVec;
   typedef std::map<std::string, BranchPtr> BranchMap;

   typedef std::vector<GenPtr> GenVec;
   typedef std::map<std::string, GenPtr> GenMap;

   typedef std::vector<ZipPtr> ZipVec;
   typedef std::map<std::string, ZipPtr> ZipMap;

   typedef std::vector<NodePtr> NodeVec;
   typedef std::map<std::string, NodePtr> NodeMap;
   
   typedef std::vector<ArcPtr> ArcVec;
   typedef std::map<std::string, ArcPtr> ArcMap;

   class Node
   {
      friend class Network;

      public:
         ConstBusPtr bus() const {return bus_;}
         BusPtr bus() {return bus_;}

         const GenVec gens() const {return gens_;}
         arma::Col<Complex> SGen() const;
         double JGen() const;

         const ZipVec zips() const {return zips_;}
         arma::Col<Complex> YZip() const;
         arma::Col<Complex> IZip() const;
         arma::Col<Complex> SZip() const;

      private:
         Node(BusPtr bus) : bus_{bus} {}

      private:
         BusPtr bus_;
         GenVec gens_;
         ZipVec zips_;
   };

   class Arc
   {
      friend class Network;

      public:
         ConstBranchPtr branch() const {return branch_;}
         BranchPtr branch() {return branch_;}
         ConstNodePtr node0() const {return node0_;}
         NodePtr node0() {return node0_;}
         ConstNodePtr node1() const {return node1_;}
         NodePtr node1() {return node1_;}
         
      private:
         Arc(BranchPtr branch, NodePtr node0, NodePtr node1) : branch_{branch}, node0_(node0), node1_(node1)
         {
            // Empty.
         }

      private:
         BranchPtr branch_;
         NodePtr node0_;
         NodePtr node1_;
   };

   class Network : public Component
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Network(const std::string& id, double PBase = 1.0);

      /// @}
      
      /// @name Component Type:
      /// @{
         
         static constexpr const char* sComponentType()
         {
            return "network";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      /// @}

      /// @name Network Attributes:
      /// @{
         
         double PBase() const
         {
            return PBase_;
         }
         
         virtual void setPBase(double PBase)
         {
            PBase_ = PBase;
         }

         double nomFreq() const
         {
            return nomFreq_;
         }

         virtual void setNomFreq(double nomFreq)
         {
            nomFreq_ = nomFreq;
         }

         double freq() const
         {
            return freq_;
         }

         virtual void setFreq(double freq)
         {
            freq_ = freq;
         }

      /// @}
         
      /// @name Network Components:
      /// @{
         
         const NodeVec& nodes() const
         {
            return nodeVec_;
         }
         ConstNodePtr node(const std::string& id) const
         {
            NodeMap::const_iterator it = nodeMap_.find(id);
            return (it == nodeMap_.end()) ? nullptr : it->second;
         }
         NodePtr node(const std::string& id)
         {
            return std::const_pointer_cast<Node>((static_cast<const Network*>(this))->node(id));
         }
         virtual void addNode(BusPtr bus);

         const ArcVec& arcs() const
         {
            return arcVec_;
         }
         ConstArcPtr arc(const std::string& id) const
         {
            ArcMap::const_iterator it = arcMap_.find(id);
            return (it == arcMap_.end()) ? nullptr : it->second;
         }
         ArcPtr arc(const std::string& id)
         {
            return std::const_pointer_cast<Arc>((static_cast<const Network*>(this))->arc(id));
         }
         virtual void addArc(BranchPtr branch, const std::string& bus0Id, const std::string& bus1Id);
         
         const GenVec& gens() const
         {
            return genVec_;
         }
         ConstGenPtr gen(const std::string& id) const
         {
            GenMap::const_iterator it = genMap_.find(id);
            return (it == genMap_.end()) ? nullptr : it->second;
         }
         GenPtr gen(const std::string& id)
         {
            return std::const_pointer_cast<GenInterface>((static_cast<const Network*>(this))->gen(id));
         }
         virtual void addGen(GenPtr gen, const std::string& busId);

         const ZipVec& zips() const
         {
            return zipVec_;
         }
         ConstZipPtr zip(const std::string& id) const
         {
            ZipMap::const_iterator it = zipMap_.find(id);
            return (it == zipMap_.end()) ? nullptr : it->second;
         }
         ZipPtr zip(const std::string& id)
         {
            return std::const_pointer_cast<ZipInterface>((static_cast<const Network*>(this))->zip(id));
         }
         virtual void addZip(ZipPtr zip, const std::string& busId);

      /// @}
      
      /// @name Per-Unit Conversions:
      /// @{
         
         template<typename T> T V2Pu(const T& V, double VBase) const
         {
            return V / VBase;
         }
         template<typename T> T pu2V(const T& pu, double VBase) const
         {
            return pu * VBase;
         }
         
         template<typename T> T S2Pu(const T& S) const
         {
            return S / PBase_;
         }
         template<typename T> T pu2S(const T& pu) const
         {
            return pu * PBase_;
         }
 
         template<typename T> T Y2Pu(const T& Y, double VBase) const
         {
            return Y * VBase * VBase / PBase_;
         }
         template<typename T> T pu2Y(const T& pu, double VBase) const
         {
            return pu * PBase_ / (VBase * VBase);
         }
  
         template<typename T> T Z2Pu(const T& Z, double VBase) const
         {
            return Z * PBase_ / (VBase * VBase);
         }
         template<typename T> T pu2Z(const T& pu, double VBase) const
         {
            return pu * VBase * VBase / PBase_;
         }
  
         template<typename T> T I2Pu(const T& I, double VBase) const
         {
            return I * VBase / PBase_;
         }
         template<typename T> T pu2I(const T& pu, double VBase) const
         {
            return pu * PBase_ / VBase;
         }
         
      /// @}
         
      /// @name Power-Flow problem:
      /// @{
         
         virtual void solvePowerFlow();
         
      /// @}
      
      protected:

         virtual void print(std::ostream& os) const override;
      
      private:
         double PBase_ = 1.0;
         double nomFreq_ = 50.0;
         double freq_ = 50.0;

         NodeVec nodeVec_;
         NodeMap nodeMap_;

         ArcVec arcVec_;
         ArcMap arcMap_;

         GenVec genVec_;
         GenMap genMap_;

         ZipVec zipVec_;
         ZipMap zipMap_;
   };
}

#endif // NETWORK_DOT_H
