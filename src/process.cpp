#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

// TODO: Return this process's ID
int Process::Pid()
{
  return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization()
{
  float res;
  long active = static_cast<float>(LinuxParser::ActiveJiffies(pid_));
  long total_s = static_cast<float>(LinuxParser::UpTime(pid_));

  res = static_cast<float>(active - pre_active_) /
        static_cast<float>(total_s - pre_total_s_) /
        static_cast<float>(sysconf(_SC_CLK_TCK));
  pre_active_ = active;
  pre_total_s_ = total_s;

  return res;
}

// TODO: Return the command that generated this process
string Process::Command()
{
  return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram()
{
  return LinuxParser::Ram(pid_);
}

// TODO: Return the user (name) that generated this process
string Process::User()
{
  return LinuxParser::User(pid_);
}


// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const
{
  return LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const
{
  float ego = static_cast<float>(LinuxParser::ActiveJiffies(pid_)) /
              static_cast<float>(LinuxParser::UpTime(pid_));
  float target = static_cast<float>(LinuxParser::ActiveJiffies(a.pid_)) /
                static_cast<float>(LinuxParser::UpTime(a.pid_));
  return ego < target;
}

