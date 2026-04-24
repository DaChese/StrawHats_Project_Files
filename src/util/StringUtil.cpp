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
 * Purpose: Shared string helpers used across repos and controllers.
 */

// Primary owner: all (shared utility)

#include "util/StringUtil.h"
#include <cctype>
using namespace std;

namespace strutil {

string trim(const string& s) {
  size_t start = 0;
  while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) start++;

  size_t end = s.size();
  while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) end--;

  return s.substr(start, end - start);
}

bool isDigits(const string& s) {
  if (s.empty()) return false;
  for (char c : s) {
    if (!isdigit(static_cast<unsigned char>(c))) return false;
  }
  return true;
}

} // namespace strutil
