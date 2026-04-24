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

#include "ui/ProviderTerminalUI.h"
#include "models/ServiceRecord.h"
#include "generators/ProviderDirectoryGenerator.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

static const string DIV  = "  " + string(50, '-');
static const string DIV2 = string(52, '=');

// ── helpers ───────────────────────────────────────────────────────────────────

static string currentTimestamp() {
  auto now  = chrono::system_clock::now();
  time_t t  = chrono::system_clock::to_time_t(now);
  tm tm_buf;
#ifdef _WIN32
  localtime_s(&tm_buf, &t);
#else
  localtime_r(&t, &tm_buf);
#endif
  ostringstream oss;
  oss << put_time(&tm_buf, "%m-%d-%Y %H:%M:%S");
  return oss.str();
}

// ── ctor ──────────────────────────────────────────────────────────────────────

ProviderTerminalUI::ProviderTerminalUI(const ProviderLoginController&    loginCtrl,
                                       const MemberValidationController& memberCtrl,
                                       ServiceEntryController&           serviceCtrl,
                                       const ProviderDirectoryGenerator& dirGen,
                                       const string&                     outputDir)
  : loginCtrl_(loginCtrl), memberCtrl_(memberCtrl), serviceCtrl_(serviceCtrl),
    dirGen_(dirGen), outputDir_(outputDir) {}

// ── prompt ────────────────────────────────────────────────────────────────────

string ProviderTerminalUI::prompt(const string& label) {
  cout << label;
  string s;
  getline(cin, s);
  return s;
}

// ── provider session ──────────────────────────────────────────────────────────

void ProviderTerminalUI::runProviderSession(const Provider& provider) {
  cout << "\n" << DIV << "\n"
       << "  Logged in as : " << provider.name << "\n"
       << "  Provider #   : " << provider.providerNumber << "\n"
       << DIV << "\n\n";

  while (true) {

    // ── Session menu ─────────────────────────────────────────────────────
    cout << "  What would you like to do?\n"
         << "    [1] Submit Service Record\n"
         << "    [2] Request Provider Directory\n"
         << "    [q] Log out\n\n";

    string choice = prompt("  Choice: ");
    cout << "\n";

    if (choice == "q" || choice == "Q") {
      cout << "  [NAV] Logout\n\n";
      return;
    }

    // ── UC05: Provider Directory ─────────────────────────────────────────
    if (choice == "2") {
      string path = dirGen_.generate(outputDir_);
      if (path.empty())
        cout << "  [DIR_FAIL] Could not write directory. Check output folder.\n\n";
      else
        cout << "  [DIR_OK] Provider Directory created: " << path << "\n\n";
      continue;
    }

    if (choice != "1") {
      cout << "  Invalid choice.\n\n";
      continue;
    }

    // ── Step 1: Member # ─────────────────────────────────────────────────
    cout << DIV << "\n"
         << "  Step 1 of 4 — Member Validation\n"
         << DIV << "\n\n";

    string memberNum = prompt("  Member # (9 digits)  b=back  q=logout : ");
    cout << "\n";

    if (memberNum == "b" || memberNum == "B") { cout << "  [NAV] Back\n\n";   continue; }
    if (memberNum == "q" || memberNum == "Q") { cout << "  [NAV] Logout\n\n"; return;   }

    auto val = memberCtrl_.validate(memberNum);

    if (val.status == MemberStatusResult::INVALID_INPUT ||
        val.status == MemberStatusResult::NOT_FOUND) {
      cout << "  [MEMBER_FAIL] " << val.message << "\n\n";
      continue;
    }
    if (val.status == MemberStatusResult::SUSPENDED) {
      cout << "  [MEMBER_SUSPENDED] " << val.message << "\n\n";
      continue;
    }

    cout << "  [MEMBER_OK] " << val.message << "\n\n";

    // ── Step 2: Date of service ──────────────────────────────────────────
    cout << DIV << "\n"
         << "  Step 2 of 4 — Date of Service\n"
         << DIV << "\n\n";

    string dateOfService;
    while (true) {
      dateOfService = prompt("  Date of service (MM-DD-YYYY)  b=back  q=logout : ");
      cout << "\n";

      if (dateOfService == "q" || dateOfService == "Q") { cout << "  [NAV] Logout\n\n"; return;   }
      if (dateOfService == "b" || dateOfService == "B") { cout << "  [NAV] Back\n\n";   break;    }

      if (serviceCtrl_.validateDate(dateOfService)) {
        cout << "  [DATE_OK] Date accepted.\n\n";
        break;
      }
      cout << "  [DATE_FAIL] Invalid date format. Use MM-DD-YYYY.\n\n";
    }
    if (dateOfService == "b" || dateOfService == "B") continue;

    // ── Step 3: Service code ─────────────────────────────────────────────
    cout << DIV << "\n"
         << "  Step 3 of 4 — Service Code\n"
         << DIV << "\n\n";

    ServiceEntryResult svcResult;
    string serviceCode;
    while (true) {
      serviceCode = prompt("  Service code (6 digits)  b=back  q=logout : ");
      cout << "\n";

      if (serviceCode == "q" || serviceCode == "Q") { cout << "  [NAV] Logout\n\n"; return; }
      if (serviceCode == "b" || serviceCode == "B") { cout << "  [NAV] Back\n\n";   break;  }

      svcResult = serviceCtrl_.lookupServiceCode(serviceCode);
      if (svcResult.success) {
        cout << "  [CODE_OK] Service confirmed.\n\n";
        break;
      }
      cout << "  [CODE_FAIL] " << svcResult.message << "\n\n";
    }
    if (serviceCode == "b" || serviceCode == "B") continue;

    // ── Step 4: Confirm + comments ───────────────────────────────────────
    cout << DIV << "\n"
         << "  Step 4 of 4 — Confirm Service\n"
         << DIV << "\n\n"
         << "  Service  : " << svcResult.serviceName << "\n"
         << "  Fee      : $" << fixed << setprecision(2) << svcResult.fee << "\n"
         << "  Member # : " << memberNum << "\n"
         << "  Date     : " << dateOfService << "\n\n"
         << "  [SERVICE_FOUND] " << svcResult.serviceName
         << " — fee: $" << fixed << setprecision(2) << svcResult.fee << "\n\n";

    string confirm = prompt("  Confirm service? (y/n) : ");
    cout << "\n";

    if (confirm != "y" && confirm != "Y") {
      cout << "  [CANCELLED] Cancelled. No record saved.\n\n";
      continue;
    }

    string comments = prompt("  Comments (Enter to skip, max 100 chars) : ");
    if (comments.size() > 100) comments = comments.substr(0, 100);
    cout << "\n";

    // ── Save ─────────────────────────────────────────────────────────────
    ServiceRecord rec;
    rec.timestamp      = currentTimestamp();
    rec.dateOfService  = dateOfService;
    rec.providerNumber = provider.providerNumber;
    rec.memberNumber   = memberNum;
    rec.serviceCode    = serviceCode;
    rec.fee            = svcResult.fee;
    rec.comments       = comments;

    if (serviceCtrl_.saveRecord(rec))
      cout << "  [RECORD_SAVED] Service record saved successfully.\n\n";
    else
      cout << "  [RECORD_FAIL] Save failed.\n\n";
  }
}

// ── main login loop ───────────────────────────────────────────────────────────

void ProviderTerminalUI::run() {
  cout << "\n" << DIV2 << "\n"
       << "   Straw Hats ChocAn — Provider Terminal\n"
       << DIV2 << "\n\n";

  while (true) {
    string providerNum = prompt("  Provider # (9 digits)  q=quit : ");
    if (providerNum == "q" || providerNum == "Q") break;

    string pin = prompt("  PIN (4 digits)               : ");
    cout << "\n";

    auto login = loginCtrl_.login(providerNum, pin);
    if (!login.success) {
      cout << "  [LOGIN_FAIL] " << login.message << "\n\n";
      continue;
    }

    cout << "  [LOGIN_SUCCESS] " << login.message << "\n";
    runProviderSession(*login.provider);
    cout << "[LOGGED_OUT] Session ended.\n\n"
         << DIV2 << "\n\n";
  }

  cout << "\n  Goodbye.\n\n";
}
