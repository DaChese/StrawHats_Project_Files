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

#include "ui/OperatorTerminalUI.h"
#include <iostream>
using namespace std;

static const string DIV  = "  " + string(50, '-');
static const string DIV2 = string(52, '=');

OperatorTerminalUI::OperatorTerminalUI(OperatorController& ctrl, const string& dataDir)
  : ctrl_(ctrl), dataDir_(dataDir) {}

string OperatorTerminalUI::prompt(const string& label) {
  cout << label;
  string s;
  getline(cin, s);
  return s;
}

// ── Member CRUD ───────────────────────────────────────────────────────────────

void OperatorTerminalUI::manageMembersMenu() {
  while (true) {
    cout << "\n" << DIV << "\n"
         << "  Manage Members\n"
         << DIV << "\n\n"
         << "    [1] Add Member\n"
         << "    [2] Update Member\n"
         << "    [3] Delete Member\n"
         << "    [b] Back\n\n";

    string c = prompt("  Choice: ");
    cout << "\n";
    if (c == "b" || c == "B") return;

    if (c == "1") {
      cout << DIV << "\n  Add Member\n" << DIV << "\n\n";
      string num    = prompt("  Member # (9 digits)          : ");
      string name   = prompt("  Name                         : ");
      string status = prompt("  Status (VALIDATED/SUSPENDED) : ");
      cout << "\n";
      auto r = ctrl_.addMember(num, name, status);
      cout << "  " << (r.success ? "[CRUD_OK] " : "[CRUD_FAIL] ") << r.message << "\n\n";

    } else if (c == "2") {
      cout << DIV << "\n  Update Member\n" << DIV << "\n\n";
      string num    = prompt("  Member # to update           : ");
      string name   = prompt("  New name                     : ");
      string status = prompt("  New status (VALIDATED/SUSPENDED) : ");
      cout << "\n";
      auto r = ctrl_.updateMember(num, name, status);
      cout << "  " << (r.success ? "[CRUD_OK] " : "[CRUD_FAIL] ") << r.message << "\n\n";

    } else if (c == "3") {
      cout << DIV << "\n  Delete Member\n" << DIV << "\n\n";
      string num = prompt("  Member # to delete : ");
      cout << "\n";
      auto r = ctrl_.deleteMember(num);
      cout << "  " << (r.success ? "[CRUD_OK] " : "[CRUD_FAIL] ") << r.message << "\n\n";

    } else {
      cout << "  Invalid choice.\n\n";
    }
  }
}

// ── Provider CRUD ─────────────────────────────────────────────────────────────

void OperatorTerminalUI::manageProvidersMenu() {
  while (true) {
    cout << "\n" << DIV << "\n"
         << "  Manage Providers\n"
         << DIV << "\n\n"
         << "    [1] Add Provider\n"
         << "    [2] Update Provider\n"
         << "    [3] Delete Provider\n"
         << "    [b] Back\n\n";

    string c = prompt("  Choice: ");
    cout << "\n";
    if (c == "b" || c == "B") return;

    if (c == "1") {
      cout << DIV << "\n  Add Provider\n" << DIV << "\n\n";
      string num  = prompt("  Provider # (9 digits) : ");
      string name = prompt("  Name                  : ");
      string pin  = prompt("  PIN (4 digits)        : ");
      cout << "\n";
      auto r = ctrl_.addProvider(num, name, pin);
      cout << "  " << (r.success ? "[CRUD_OK] " : "[CRUD_FAIL] ") << r.message << "\n\n";

    } else if (c == "2") {
      cout << DIV << "\n  Update Provider\n" << DIV << "\n\n";
      string num  = prompt("  Provider # to update  : ");
      string name = prompt("  New name              : ");
      string pin  = prompt("  New PIN (4 digits)    : ");
      cout << "\n";
      auto r = ctrl_.updateProvider(num, name, pin);
      cout << "  " << (r.success ? "[CRUD_OK] " : "[CRUD_FAIL] ") << r.message << "\n\n";

    } else if (c == "3") {
      cout << DIV << "\n  Delete Provider\n" << DIV << "\n\n";
      string num = prompt("  Provider # to delete : ");
      cout << "\n";
      auto r = ctrl_.deleteProvider(num);
      cout << "  " << (r.success ? "[CRUD_OK] " : "[CRUD_FAIL] ") << r.message << "\n\n";

    } else {
      cout << "  Invalid choice.\n\n";
    }
  }
}

// ── Main menu ─────────────────────────────────────────────────────────────────

void OperatorTerminalUI::run() {
  cout << "\n" << DIV2 << "\n"
       << "   Straw Hats ChocAn — Operator Terminal\n"
       << DIV2 << "\n\n";

  while (true) {
    cout << "  What would you like to do?\n"
         << "    [1] Manage Members\n"
         << "    [2] Manage Providers\n"
         << "    [3] Import Acme Update File\n"
         << "    [q] Quit\n\n";

    string c = prompt("  Choice: ");
    cout << "\n";

    if (c == "q" || c == "Q") break;

    if (c == "1") {
      manageMembersMenu();

    } else if (c == "2") {
      manageProvidersMenu();

    } else if (c == "3") {
      cout << DIV << "\n"
           << "  Import Acme Update File\n"
           << DIV << "\n\n";
      string path = prompt("  File path  Enter = " + dataDir_ + "/acme_updates.csv : ");
      if (path.empty()) path = dataDir_ + "/acme_updates.csv";
      cout << "\n";
      auto r = ctrl_.importAcmeUpdate(path);
      cout << "  " << (r.success ? "[UPDATE_OK] " : "[UPDATE_FAIL] ") << r.message << "\n\n";

    } else {
      cout << "  Invalid choice.\n\n";
    }
  }

  cout << "\n  Goodbye.\n\n";
}
