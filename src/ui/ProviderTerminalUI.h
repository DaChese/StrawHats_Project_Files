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
 * Purpose: Provider terminal UI — login loop, session menu, service entry flow, directory request.
 */

// Primary owner: Aldo Medina (UC02 / provider terminal flow)

#pragma once
#include <string>
using namespace std;
#include "controllers/ProviderLoginController.h"
#include "controllers/MemberValidationController.h"
#include "controllers/ServiceEntryController.h"
#include "generators/ProviderDirectoryGenerator.h"

class ProviderTerminalUI {
public:
  ProviderTerminalUI(const ProviderLoginController&    loginCtrl,
                     const MemberValidationController& memberCtrl,
                     ServiceEntryController&           serviceCtrl,
                     const ProviderDirectoryGenerator& dirGen,
                     const string&                     outputDir);
  void run();

private:
  const ProviderLoginController&    loginCtrl_;
  const MemberValidationController& memberCtrl_;
  ServiceEntryController&           serviceCtrl_;
  const ProviderDirectoryGenerator& dirGen_;
  string outputDir_;

  string prompt(const string& label);
  void runProviderSession(const Provider& provider);
};
