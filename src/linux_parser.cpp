#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::getline;
using std::ifstream;
using std::istringstream;
using std::stringstream;
using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::replace;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      replace(line.begin(), line.end(), ' ', '_');
      replace(line.begin(), line.end(), '=', ' ');
      replace(line.begin(), line.end(), '"', ' ');
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return version;
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
float LinuxParser::MemoryUtilization() {
  float memTotal, memFree;
  string line;
  ifstream filestream(LinuxParser::kProcDirectory +
                      LinuxParser::kMeminfoFilename);

  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      replace(line.begin(), line.end(), ':', ' ');
      istringstream linestream(line);
      string key;
      float value;
      linestream >> key >> value;
      if (key == "MemTotal") {
        memTotal = value;
      }
      if (key == "MemFree") {
        memFree = value;
      }
    }
  }
  return (memTotal - memFree) / memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  ifstream filestream(LinuxParser::kProcDirectory +
                      LinuxParser::kUptimeFilename);
  long uptime, idle;
  if (filestream.is_open()) {
    filestream >> uptime >> idle;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
    ifstream file(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    string line, key;
    double kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_, kGuest_, kGuestNice_;
    getline(file, line);
    stringstream line_stream(line);
    line_stream >> key >> kUser_ >> kNice_ >> kSystem_ >> kIdle_ >> kIOwait_ >> kIRQ_ >> kSoftIRQ_ >> kSteal_ >> kGuest_ >> kGuestNice_;
    double idle = kIdle_ + kIOwait_;
    double nonIdle = kUser_ + kNice_ + kSystem_ + kIRQ_ + kSoftIRQ_ + kSteal_;
    double total = idle + nonIdle;

    return nonIdle / total;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  long value = 0;
  ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key;
  int value = 0;
  ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line, cmd;
  ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) +
                      LinuxParser::kCmdlineFilename);
  if (filestream.is_open()) {
    if (getline(filestream, line)) {
      return line;
    }
  }

  return "";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  ifstream file(LinuxParser::kProcDirectory + to_string(pid) +
                LinuxParser::kStatusFilename);
  string line, key, value;
  while (getline(file, line)) {
    replace(line.begin(), line.end(), ':', ' ');
    istringstream str_stream(line);
    if (str_stream >> key >> value) {
      if (key == "VmSize") return to_string(std::stol(value) / 1000);
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  ifstream file(LinuxParser::kProcDirectory + to_string(pid) +
                LinuxParser::kStatusFilename);
  string line, key, value;
  while (getline(file, line)) {
    replace(line.begin(), line.end(), ':', ' ');
    istringstream str_stream(line);
    if (str_stream >> key >> value) {
      if (key == "Uid") return value;
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  ifstream filestream(LinuxParser::kPasswordPath);
  string line, key, x, value;
  while (getline(filestream, line)) {
    replace(line.begin(), line.end(), ':', ' ');
    istringstream strstream(line);
    strstream >> key >> x >> value;
    if (value == uid) {
      break;
    }
  }

  return key;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  ifstream file(LinuxParser::kProcDirectory + to_string(pid) +
                LinuxParser::kStatFilename);
  string val, line;
  if (getline(file, line)) {
    istringstream str_stream(line);
    for (u_int i = 0; i <= 21; i++) {
      str_stream >> val;
    }
    return std::stol(val);
  }

  return 0l;
}