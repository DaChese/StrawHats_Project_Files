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

#pragma once
#include <string>
using namespace std;
#include "repositories/MemberRepository.h"
#include "repositories/ProviderRepository.h"

struct CrudResult {
  bool   success = false;
  string message;
};

class OperatorController {
public:
  OperatorController(MemberRepository&   memberRepo,
                     ProviderRepository& providerRepo,
                     const string&       membersPath,
                     const string&       providersPath);

  // Member CRUD
  CrudResult addMember(const string& number, const string& name, const string& status);
  CrudResult updateMember(const string& number, const string& name, const string& status);
  CrudResult deleteMember(const string& number);

  // Provider CRUD
  CrudResult addProvider(const string& number, const string& name, const string& pin);
  CrudResult updateProvider(const string& number, const string& name, const string& pin);
  CrudResult deleteProvider(const string& number);

  // Acme nightly update import
  // File format: memberNumber,newStatus  (header row expected)
  CrudResult importAcmeUpdate(const string& updateFilePath);

private:
  MemberRepository&   memberRepo_;
  ProviderRepository& providerRepo_;
  string membersPath_;
  string providersPath_;

  bool validId_(const string& id) const;   // 9 digits
  bool validPin_(const string& pin) const; // 4 digits
};
