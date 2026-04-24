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

#pragma once
#include <string>
using namespace std;
#include "generators/WeeklyBatchGenerator.h"

class ManagerTerminalUI {
public:
  ManagerTerminalUI(const WeeklyBatchGenerator& batchGen,
                    const string& recordsPath,
                    const string& outputDir);
  void run();

private:
  const WeeklyBatchGenerator& batchGen_;
  string recordsPath_;
  string outputDir_;

  string prompt(const string& label);
};
