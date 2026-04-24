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
 * Purpose: Minimal CSV parser — splits a single line, handles quoted fields.
 */

// Primary owner: all (shared utility)

#include "util/Csv.h"
using namespace std;

namespace csv {

vector<string> splitLine(const string& line) {
  vector<string> out;
  string cur;
  bool inQuotes = false;

  for (size_t i = 0; i < line.size(); i++) {
    char c = line[i];

    if (c == '"') {
      // handle escaped quotes ("")
      if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
        cur.push_back('"');
        i++;
      } else {
        inQuotes = !inQuotes;
      }
    } else if (c == ',' && !inQuotes) {
      out.push_back(cur);
      cur.clear();
    } else {
      cur.push_back(c);
    }
  }
  out.push_back(cur);
  return out;
}

} // namespace csv
