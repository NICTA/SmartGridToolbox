#ifndef OVERHEAD_LINE_DOT_H
#define OVERHEAD_LINE_DOT_H

#include <SmartGridToolbox/Branch.h>

namespace SmartGridToolbox
{
   class OverheadLine : public Branch
   {
      public:
         OverheadLine(const std::string & name, const Phases & phases0, const Phases & phases1, double length,
                      int nNeutral, ublas::vector<double> lineResistivity, double earthResistivity,
                      ublas::matrix<double> distMat, double freq); 
      
      private:
         void recalcY();

      private:
         double L_;                      ///< Length.
         int nNeutral_;                  ///< Internal multigrounded neutral lines.
         ublas::vector<double> rhoLine_; ///< Line resistivity.
         double rhoEarth_;               ///< Ground resistivity.
         ublas::matrix<double> Dij_;     ///< Distance between lines, diagonal = GMR.
         double f_;                      ///< Frequency : TODO : link to network frequency.
   };
}

#endif // OVERHEAD_LINE_DOT_H
