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
 * Purpose: Manager terminal UI — triggers weekly batch run and displays output path (UC04).
 */

// Primary owner: Audity Moumi (UC04 / manager terminal)

#include "ui/ManagerTerminalUI.h"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;
namespace fs = filesystem;

static const string DIV  = "  " + string(50, '-');
static const string DIV2 = string(52, '=');

ManagerTerminalUI::ManagerTerminalUI(const WeeklyBatchGenerator& batchGen,
                                     const string& recordsPath,
                                     const string& outputDir)
  : batchGen_(batchGen), recordsPath_(recordsPath), outputDir_(outputDir) {}

string ManagerTerminalUI::prompt(const string& label) {
  cout << label;
  string s;
  getline(cin, s);
  return s;
}

static string todayMDY() {
  auto now = chrono::system_clock::now();
  time_t t = chrono::system_clock::to_time_t(now);
  tm tm_buf;
#ifdef _WIN32
  localtime_s(&tm_buf, &t);
#else
  localtime_r(&t, &tm_buf);
#endif
  ostringstream oss;
  oss << put_time(&tm_buf, "%m-%d-%Y");
  return oss.str();
}

void ManagerTerminalUI::run() {
  cout << "\n" << DIV2 << "\n"
       << "   Straw Hats ChocAn — Manager Terminal\n"
       << DIV2 << "\n\n";

  while (true) {
    cout << "  What would you like to do?\n"
         << "    [1] Run Weekly Batch (UC04)\n"
         << "    [q] Quit\n\n";

    string choice = prompt("  Choice: ");
    cout << "\n";

    if (!cin || choice == "q" || choice == "Q") break;
    if (choice != "1") { cout << "  Invalid choice.\n\n"; continue; }

    // ── Weekly batch ─────────────────────────────────────────────────────
    cout << DIV << "\n"
         << "  Weekly Batch — Report Generation\n"
         << DIV << "\n\n";

    string today   = todayMDY();
    string weekEnd = prompt("  Week ending date (MM-DD-YYYY)  Enter = " + today + " : ");
    if (!cin) weekEnd = today;
    if (weekEnd.empty()) weekEnd = today;
    cout << "\n";

    // Convert MM-DD-YYYY → YYYYMMDD for folder name
    string folderTag = weekEnd;
    if (folderTag.size() == 10)
      folderTag = folderTag.substr(6,4) + folderTag.substr(0,2) + folderTag.substr(3,2);

    string reportDir = outputDir_ + "/reports/week_" + folderTag;

    if (batchGen_.run(recordsPath_, weekEnd, reportDir))
      cout << "  [BATCH_OK] Reports generated in: " << reportDir << "\n\n";
    else
      cout << "  [BATCH_FAIL] Invalid date or could not create output folder.\n\n";
  }

  cout << "\n  Goodbye.\n\n";
}
