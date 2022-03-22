#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization()
{
  float res;
  long total = LinuxParser::Jiffies();
  long active = LinuxParser::ActiveJiffies();

  res = static_cast<float>(active - pre_active_) /
        static_cast<float>(total - pre_total_);

  pre_total_ = total;
  pre_active_ = active;
  return res;
}