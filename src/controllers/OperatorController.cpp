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
 * Purpose: Member + provider CRUD and Acme nightly status import for the Operator terminal.
 */

// Primary owner: Lulu Chen (UC01 / data management)

#include "controllers/OperatorController.h"
#include "util/Csv.h"
#include "util/StringUtil.h"
#include <fstream>
using namespace std;

OperatorController::OperatorController(MemberRepository&   memberRepo,
                                       ProviderRepository& providerRepo,
                                       const string&       membersPath,
                                       const string&       providersPath)
  : memberRepo_(memberRepo), providerRepo_(providerRepo),
    membersPath_(membersPath), providersPath_(providersPath) {}

bool OperatorController::validId_(const string& id) const {
  return strutil::isDigits(id) && id.size() == 9;
}
bool OperatorController::validPin_(const string& pin) const {
  return strutil::isDigits(pin) && pin.size() == 4;
}

// ── Member CRUD ───────────────────────────────────────────────────────────────

CrudResult OperatorController::addMember(const string& number,
                                         const string& name,
                                         const string& status) {
  if (!validId_(number))   return {false, "Member number must be 9 digits."};
  if (name.empty())        return {false, "Name cannot be empty."};
  string s = status;
  for (auto& c : s) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
  if (s != "VALIDATED" && s != "SUSPENDED")
    return {false, "Status must be VALIDATED or SUSPENDED."};

  Member m{number, name, s};
  if (!memberRepo_.add(m)) return {false, "Member " + number + " already exists."};
  memberRepo_.saveToCsv(membersPath_);
  return {true, "Member " + number + " added."};
}

CrudResult OperatorController::updateMember(const string& number,
                                            const string& name,
                                            const string& status) {
  if (!validId_(number)) return {false, "Member number must be 9 digits."};
  if (name.empty())      return {false, "Name cannot be empty."};
  string s = status;
  for (auto& c : s) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
  if (s != "VALIDATED" && s != "SUSPENDED")
    return {false, "Status must be VALIDATED or SUSPENDED."};

  Member m{number, name, s};
  if (!memberRepo_.update(m)) return {false, "Member " + number + " not found."};
  memberRepo_.saveToCsv(membersPath_);
  return {true, "Member " + number + " updated."};
}

CrudResult OperatorController::deleteMember(const string& number) {
  if (!validId_(number)) return {false, "Member number must be 9 digits."};
  if (!memberRepo_.remove(number)) return {false, "Member " + number + " not found."};
  memberRepo_.saveToCsv(membersPath_);
  return {true, "Member " + number + " deleted."};
}

// ── Provider CRUD ─────────────────────────────────────────────────────────────

CrudResult OperatorController::addProvider(const string& number,
                                           const string& name,
                                           const string& pin) {
  if (!validId_(number))  return {false, "Provider number must be 9 digits."};
  if (name.empty())       return {false, "Name cannot be empty."};
  if (!validPin_(pin))    return {false, "PIN must be 4 digits."};

  Provider p{number, name, pin};
  if (!providerRepo_.add(p)) return {false, "Provider " + number + " already exists."};
  providerRepo_.saveToCsv(providersPath_);
  return {true, "Provider " + number + " added."};
}

CrudResult OperatorController::updateProvider(const string& number,
                                              const string& name,
                                              const string& pin) {
  if (!validId_(number)) return {false, "Provider number must be 9 digits."};
  if (!validPin_(pin))   return {false, "PIN must be 4 digits."};

  Provider p{number, name, pin};
  if (!providerRepo_.update(p)) return {false, "Provider " + number + " not found."};
  providerRepo_.saveToCsv(providersPath_);
  return {true, "Provider " + number + " updated."};
}

CrudResult OperatorController::deleteProvider(const string& number) {
  if (!validId_(number)) return {false, "Provider number must be 9 digits."};
  if (!providerRepo_.remove(number)) return {false, "Provider " + number + " not found."};
  providerRepo_.saveToCsv(providersPath_);
  return {true, "Provider " + number + " deleted."};
}

// ── Acme nightly update import ────────────────────────────────────────────────

CrudResult OperatorController::importAcmeUpdate(const string& updateFilePath) {
  ifstream in(updateFilePath);
  if (!in.is_open())
    return {false, "Cannot open update file: " + updateFilePath};

  string line;
  bool first = true;
  int updated = 0, skipped = 0;

  while (getline(in, line)) {
    if (first) { first = false; continue; } // skip header
    if (line.empty()) continue;

    auto cols = csv::splitLine(line);
    if (cols.size() < 2) { skipped++; continue; }

    string number = strutil::trim(cols[0]);
    string status = strutil::trim(cols[1]);
    for (auto& c : status) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

    if (!validId_(number)) { skipped++; continue; }
    if (status != "VALIDATED" && status != "SUSPENDED") { skipped++; continue; }

    auto existing = memberRepo_.findByNumber(number);
    if (!existing.has_value()) {
      // log but don't crash
      skipped++;
      continue;
    }
    Member m = *existing;
    m.status = status;
    memberRepo_.update(m);
    updated++;
  }

  memberRepo_.saveToCsv(membersPath_);
  return {true, "Import complete. Updated: " + to_string(updated)
                + ", Skipped: " + to_string(skipped) + "."};
}
