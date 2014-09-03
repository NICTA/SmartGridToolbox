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

   typedef std::vector<std::shared_ptr<Bus>> BusVec;
   typedef std::map<std::string, std::shared_ptr<Bus>> BusMap;

   typedef std::vector<std::shared_ptr<BranchAbc>> BranchVec;
   typedef std::map<std::string, std::shared_ptr<BranchAbc>> BranchMap;

   typedef std::vector<std::shared_ptr<GenAbc>> GenVec;
   typedef std::map<std::string, std::shared_ptr<GenAbc>> GenMap;

   typedef std::vector<std::shared_ptr<ZipAbc>> ZipVec;
   typedef std::map<std::string, std::shared_ptr<ZipAbc>> ZipMap;
   
   typedef std::vector<std::shared_ptr<Node>> NodeVec;
   typedef std::map<std::string, std::shared_ptr<Node>> NodeMap;
   
   typedef std::vector<std::shared_ptr<Arc>> ArcVec;
   typedef std::map<std::string, std::shared_ptr<Arc>> ArcMap;

   class Node
   {
      friend class Network;

      public:
         std::shared_ptr<Bus> bus() {return bus_;}

         const GenVec gens() {return gens_;}
         ublas::vector<Complex> SGen() const;

         const ZipVec zips() {return zips_;}
         ublas::vector<Complex> YZip() const;
         ublas::vector<Complex> IZip() const;
         ublas::vector<Complex> SZip() const;

      private:
         Node(std::shared_ptr<Bus> bus) : bus_{bus} {}

      private:
         std::shared_ptr<Bus> bus_;
         GenVec gens_;
         ZipVec zips_;
   };

   class Arc
   {
      friend class Network;

      public:
         std::shared_ptr<BranchAbc> branch() {return branch_;}
         std::shared_ptr<Node> node0() {return node0_;}
         std::shared_ptr<Node> node1() {return node1_;}

      private:
         Arc(std::shared_ptr<BranchAbc> branch, std::shared_ptr<Node> node0, std::shared_ptr<Node> node1) :
            branch_{branch}, node0_(node0), node1_(node1) {}

      private:
         std::shared_ptr<BranchAbc> branch_;
         std::shared_ptr<Node> node0_;
         std::shared_ptr<Node> node1_;
   };

   class Network : public Component
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Network(const std::string& id, double PBase);

      /// @}
      
      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const {return "network";}

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
         std::shared_ptr<const Node> node(const std::string& id) const
         {
            NodeMap::const_iterator it = nodeMap_.find(id);
            return (it == nodeMap_.end()) ? nullptr : it->second;
         }
         std::shared_ptr<Node> node(const std::string& id)
         {
            return std::const_pointer_cast<Node>((const_cast<const Network*>(this))->node(id));
         }
         virtual void addNode(std::shared_ptr<Bus> bus)
         {
            auto nd = std::shared_ptr<Node>(new Node(bus)); 
            nodeMap_[bus->id()] = nd;
            nodeVec_.push_back(nd);
         }

         const ArcVec& arcs() const
         {
            return arcVec_;
         }
         std::shared_ptr<const Arc> arc(const std::string& id) const
         {
            ArcMap::const_iterator it = arcMap_.find(id);
            return (it == arcMap_.end()) ? nullptr : it->second;
         }
         std::shared_ptr<Arc> arc(const std::string& id)
         {
            return std::const_pointer_cast<Arc>((const_cast<const Network*>(this))->arc(id));
         }
         virtual void addArc(std::shared_ptr<BranchAbc> branch, const std::string& bus0Id, 
               const std::string& bus1Id)
         {
            auto nd0 = node(bus0Id); 
            auto nd1 = node(bus1Id); 
            auto arc = std::shared_ptr<Arc>(new Arc(branch, nd0, nd1));
            arcMap_[branch->id()] = arc;
            arcVec_.push_back(arc);
         }

         const GenVec& gens() const
         {
            return genVec_;
         }
         std::shared_ptr<const GenAbc> gen(const std::string& id) const
         {
            GenMap::const_iterator it = genMap_.find(id);
            return (it == genMap_.end()) ? nullptr : it->second;
         }
         std::shared_ptr<GenAbc> gen(const std::string& id)
         {
            return std::const_pointer_cast<GenAbc>((const_cast<const Network*>(this))->gen(id));
         }
         virtual void addGen(std::shared_ptr<GenAbc> gen, const std::string& busId)
         {
            genMap_[gen->id()] = gen;
            genVec_.push_back(gen);
            node(busId)->gens_.push_back(gen);
         }

         const ZipVec& zips() const
         {
            return zipVec_;
         }
         std::shared_ptr<const ZipAbc> zip(const std::string& id) const
         {
            ZipMap::const_iterator it = zipMap_.find(id);
            return (it == zipMap_.end()) ? nullptr : it->second;
         }
         std::shared_ptr<ZipAbc> zip(const std::string& id)
         {
            return std::const_pointer_cast<ZipAbc>((const_cast<const Network*>(this))->zip(id));
         }
         virtual void addZip(std::shared_ptr<ZipAbc> zip, const std::string& busId)
         {
            zipMap_[zip->id()] = zip;
            zipVec_.push_back(zip);
            node(busId)->zips_.push_back(zip);
         }

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
