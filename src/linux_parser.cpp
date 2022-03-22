#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
  string line;
  string key, unit;
  float value;
  float total_memory, free_memory;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  while (stream.is_open() && std::getline(stream, line))
  {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    while (linestream >> key >> value >> unit)
    {
      if (key == filterMemTotalString)
        total_memory = value;
      if (key == filterMemFreeString)
        free_memory = value;

    }
  }
  return (total_memory - free_memory) / total_memory;
}

// TODO: Read and return the system uptime
long int LinuxParser::UpTime()
{
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  long int time;
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> time;
  }
  return time;
}
// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  long res = 0;
  auto jiffies = CpuUtilization();
  for (const auto& jiffy : jiffies)
  {
    res += std::stol(jiffy);
  }
  return res;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid)
{
  string line, value;
  std::vector<string> value_list;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value)
    {
      value_list.push_back(value);
    }

  }
  long active_jiffies = 0;
  if (value_list.size() > 21)
  {
    active_jiffies = stol(value_list[13]) + stol(value_list[14]) + stol(value_list[15]) + stol(value_list[16]);
  }
  return active_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  return Jiffies() - IdleJiffies();
}


// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  auto jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
  vector<string> res;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, key, value;

  while (stream.is_open() && std::getline(stream, line))
  {
    std::istringstream linestream(line);
    while (linestream >> key)
    {
      if (key != filterCpu)
        continue;
      while (linestream >> value)
        res.push_back(value);
    }
  }

  return res;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  int value;
  int res = 0;
  string line, key;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    while(stream.is_open() && std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == filterProcesses)
        {
          res = value;
        }
      }
    }
  }
  return res;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  int value;
  int res = 0;
  string line, key;
  std::ifstream stream(kProcDirectory + kStatFilename);
  while(stream.is_open() && std::getline(stream, line))
  {
    std::istringstream linestream(line);
    while (linestream >> key >> value)
    {
      if (key == filterRunningProcesses)
      {
        res = value;
      }
    }
  }
  return res;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid)
{
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    return line;
  }
  return "";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid)
{
  string key, value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while(stream.is_open() && std::getline(stream, line))
  {
    std::istringstream linestream(line);
    while (linestream >> key >> value)
    {
      if (key == filterProcMem)
      {
        return std::to_string(stoi(value) / 1024);
      }
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid)
{
  string key, value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while(stream.is_open() && std::getline(stream, line))
  {
    std::istringstream linestream(line);
    while (linestream >> key >> value)
    {
      if (key == "Uid:")
      {
        return value;
      }
    }
  }
  return "";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid)
{
  string line;
  std::ifstream stream(kPasswordPath);
  string user, x, uid, id, pwd, p1, p2;
  string target_uid = Uid(pid);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid >> id >> pwd >> p1 >> p2)
      {
        if (uid == target_uid)
          return user;
      }
    }
  }
  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid)
{
  long up_time;
  string line, value;
  std::vector<string> value_list;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value)
    {
      value_list.push_back(value);
    }
  }
  // std::cout << "!!!" << value_list[21]  << std::endl;

    up_time = LinuxParser::UpTime() - stol(value_list[21]) / sysconf(_SC_CLK_TCK);


  return up_time;
}