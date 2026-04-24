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
 * Purpose: Checks member number format and status (VALIDATED / SUSPENDED / NOT_FOUND).
 */

// Primary owner: Lulu Chen (UC01 / member validation)

#include "controllers/MemberValidationController.h"
#include "util/StringUtil.h"
#include <cctype>
using namespace std;

MemberValidationController::MemberValidationController(const MemberRepository& repo)
  : repo_(repo) {}

MemberValidationResult MemberValidationController::validate(const string& memberNumber) const {
  MemberValidationResult res;

  if (!strutil::isDigits(memberNumber) || memberNumber.size() != 9) {
    res.status = MemberStatusResult::INVALID_INPUT;
    res.message = "Invalid member number (must be 9 digits).";
    return res;
  }

  auto found = repo_.findByNumber(memberNumber);
  if (!found.has_value()) {
    res.status = MemberStatusResult::NOT_FOUND;
    res.message = "Member not found.";
    return res;
  }

  string status = found->status;
  // normalize quick
  for (auto& c : status) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

  if (status == "SUSPENDED") {
    res.status = MemberStatusResult::SUSPENDED;
    res.message = "Member is suspended.";
    return res;
  }

  res.status = MemberStatusResult::VALIDATED;
  res.message = "Member is validated.";
  return res;
}
