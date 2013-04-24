#ifndef TIME_SERIES_DOT_H
#define TIME_SERIES_DOT_H

namespace SmartGridToolbox
{
   template<typename T>
   class TimeSeriesBase
   {
      public:
         virtual ~TimeSeriesBase
         {
            // Empty.
         }

         virtual T value(const ptime & timestamp) = 0;
   }

   template<typename T>
   class TimeSeriesWithData : public TimeSeriesBase
   {
      public:
         TimeSeriesWithData()
         {
            // Empty.
         }

         typedef T Value;
         typedef std::pair<ptime, T> Datum;
         typedef std::vector<DataType> Data;

         const Data & getData() const
         {
            return data_;
         }

      private:

         Storage data_;
   }
}

#endif // TIME_SERIES_DOT_H
