/*
 * Project: ChocAn Data Center System (Straw Hats)
 *
 * Team + Ownership:
 * - Aldo Medina    (UC02 / Submit Service Record; Provider Terminal flow + record save)
 * - Lulu Chen      (UC01 / Validate Member Eligibility; Data Management support)
 * - Audity Moumi   (UC04 / Weekly Batch + Reports + EFT outputs)
 * - Rania Dayekh   (UC05 / Provider Directory generation)
 *
 * Created on:   2025-04-15
 * Last updated: 2025-04-24
 * Purpose: Writes a sorted provider directory file to data/output/ (UC05).
 */

// Primary owner: Rania Dayekh (UC05 / provider directory)

#include "generators/ProviderDirectoryGenerator.h"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;
namespace fs = filesystem;

ProviderDirectoryGenerator::ProviderDirectoryGenerator(const ServiceRepository& svcRepo)
  : svcRepo_(svcRepo) {}

string ProviderDirectoryGenerator::generate(const string& outputDir) const {
  // Build datestamp YYYYMMDD
  auto now  = chrono::system_clock::now();
  time_t t  = chrono::system_clock::to_time_t(now);
  tm tm_buf;
#ifdef _WIN32
  localtime_s(&tm_buf, &t);
#else
  localtime_r(&t, &tm_buf);
#endif
  ostringstream ds;
  ds << put_time(&tm_buf, "%Y%m%d");

  // Ensure output directory exists
  error_code ec;
  fs::create_directories(outputDir, ec);
  if (ec) return "";

  string path = outputDir + "/provider_directory_" + ds.str() + ".txt";
  ofstream out(path);
  if (!out.is_open()) return "";

  auto services = svcRepo_.allSortedByName();

  out << "ChocAn Provider Directory\n";
  out << "Generated: " << put_time(&tm_buf, "%m-%d-%Y") << "\n";
  out << string(50, '-') << "\n";
  out << left << setw(30) << "Service Name"
      << setw(10) << "Code"
      << "Fee\n";
  out << string(50, '-') << "\n";

  for (auto& sc : services) {
    out << left  << setw(30) << sc.name
        << setw(10) << sc.code
        << "$" << fixed << setprecision(2) << sc.fee << "\n";
  }

  out << string(50, '-') << "\n";
  out << "Total services: " << services.size() << "\n";
  return path;
}
