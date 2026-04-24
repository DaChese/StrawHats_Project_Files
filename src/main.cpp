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
 * Purpose: Entry point — loads all repos, wires up controllers, and runs the terminal selector.
 */

// Primary owner: all (shared entry point)

#include <filesystem>
#include <iostream>
#include <string>
using namespace std;
namespace fs = filesystem;

#include "repositories/MemberRepository.h"
#include "repositories/ProviderRepository.h"
#include "repositories/ServiceRepository.h"
#include "repositories/ServiceRecordRepository.h"
#include "controllers/ProviderLoginController.h"
#include "controllers/MemberValidationController.h"
#include "controllers/ServiceEntryController.h"
#include "controllers/OperatorController.h"
#include "generators/ProviderDirectoryGenerator.h"
#include "generators/WeeklyBatchGenerator.h"
#include "ui/ProviderTerminalUI.h"
#include "ui/ManagerTerminalUI.h"
#include "ui/OperatorTerminalUI.h"

static string getArgValue(int argc, char** argv, const string& key, const string& defVal) {
  for (int i = 1; i < argc; i++) {
    string a = argv[i];
    if (a == key && i + 1 < argc) return argv[i + 1];
  }
  return defVal;
}

int main(int argc, char** argv) {
  string dataDir = getArgValue(argc, argv, "--data-dir", "data");
  string outputDir = dataDir + "/output";

  // Ensure output dir exists
  error_code ec;
  fs::create_directories(outputDir, ec);

  // ── Load repositories ─────────────────────────────────────────────────────
  MemberRepository   memberRepo;
  ProviderRepository providerRepo;
  ServiceRepository  serviceRepo;

  string membersPath      = dataDir + "/members.csv";
  string providersPath    = dataDir + "/providers.csv";
  string servicesPath     = dataDir + "/services.csv";
  string serviceRecordsPath = dataDir + "/service_records.csv";

  if (!memberRepo.loadFromCsv(membersPath)) {
    cerr << "ERROR: Could not load " << membersPath << "\n"; return 1;
  }
  if (!providerRepo.loadFromCsv(providersPath)) {
    cerr << "ERROR: Could not load " << providersPath << "\n"; return 1;
  }
  if (!serviceRepo.loadFromCsv(servicesPath)) {
    cerr << "ERROR: Could not load " << servicesPath << "\n"; return 1;
  }

  // ── Build controllers / generators ───────────────────────────────────────
  ProviderLoginController    loginCtrl(providerRepo);
  MemberValidationController memberCtrl(memberRepo);
  ServiceRecordRepository    recordRepo(serviceRecordsPath);
  ServiceEntryController     serviceCtrl(serviceRepo, recordRepo);
  OperatorController         operatorCtrl(memberRepo, providerRepo, membersPath, providersPath);
  ProviderDirectoryGenerator dirGen(serviceRepo);
  WeeklyBatchGenerator       batchGen(memberRepo, providerRepo, serviceRepo);

  // ── Top-level terminal selector ───────────────────────────────────────────
  cout << "=== Straw Hats ChocAn ===\n\n"
       << "  [1] Provider Terminal\n"
       << "  [2] Manager Terminal\n"
       << "  [3] Operator Terminal\n"
       << "  [q] Quit\n";

  while (true) {
    cout << "\nSelect terminal: ";
    string choice;
    getline(cin, choice);

    if (choice == "q" || choice == "Q") break;

    if (choice == "1") {
      ProviderTerminalUI ui(loginCtrl, memberCtrl, serviceCtrl, dirGen, outputDir);
      ui.run();
    } else if (choice == "2") {
      ManagerTerminalUI ui(batchGen, serviceRecordsPath, outputDir);
      ui.run();
    } else if (choice == "3") {
      OperatorTerminalUI ui(operatorCtrl, dataDir);
      ui.run();
    } else {
      cout << "Invalid choice.\n";
    }

    // Re-print menu after returning from a terminal
    cout << "\n=== Straw Hats ChocAn ===\n\n"
         << "  [1] Provider Terminal\n"
         << "  [2] Manager Terminal\n"
         << "  [3] Operator Terminal\n"
         << "  [q] Quit\n";
  }

  cout << "Goodbye.\n";
  return 0;
}
