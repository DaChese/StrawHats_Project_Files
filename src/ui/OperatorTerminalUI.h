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
 * Purpose: Operator terminal UI — member/provider CRUD menus and Acme import.
 */

// Primary owner: Lulu Chen (UC01 / data management)

#pragma once
#include <string>
using namespace std;
#include "controllers/OperatorController.h"

class OperatorTerminalUI {
public:
  explicit OperatorTerminalUI(OperatorController& ctrl, const string& dataDir);
  void run();

private:
  OperatorController& ctrl_;
  string dataDir_;

  string prompt(const string& label);
  void manageMembersMenu();
  void manageProvidersMenu();
};
