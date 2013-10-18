#include <chrono>

class Stopwatch
{
   public:
      Stopwatch() : running_(false), dur_(std::chrono::duration<double>::zero()) {}
      void start() {running_ = true; start_ = std::chrono::system_clock::now();}
      void stop() {if (running_) dur_ += std::chrono::system_clock::now() - start_; running_ = false;}
      void reset() {dur_ = std::chrono::duration<double>::zero();}
      double seconds() {return dur_.count();}
   private:
      bool running_;
      std::chrono::time_point<std::chrono::system_clock> start_;
      std::chrono::duration<double> dur_;
};
