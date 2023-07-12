#include <iostream>
#include <iomanip>
#include <chrono>


#define START(timename) auto timename = std::chrono::system_clock::now();
#define STOP(timename,elapsed)  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - timename).count();


class utimer {
  std::chrono::system_clock::time_point start;
  std::chrono::system_clock::time_point stop;
  using usecs = std::chrono::microseconds;
  using msecs = std::chrono::milliseconds;

private:
  long * us_elapsed;
  
public:
    
  utimer(long * us): us_elapsed(us) {
    start = std::chrono::system_clock::now();
  }

  ~utimer() {
    stop =std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    
    if(us_elapsed != NULL)
      (*us_elapsed) = musec;
  }
};