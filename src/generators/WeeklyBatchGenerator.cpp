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
 * Purpose: Generates all 5 weekly report files for a 7-day window (UC04).
 */

// Primary owner: Audity Moumi (UC04 / weekly batch + EFT)

#include "generators/WeeklyBatchGenerator.h"
#include "repositories/ServiceRecordRepository.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <cstring>
#include <sstream>
using namespace std;
namespace fs = filesystem;

// ── ctor ─────────────────────────────────────────────────────────────────────

WeeklyBatchGenerator::WeeklyBatchGenerator(const MemberRepository&   memberRepo,
                                           const ProviderRepository& providerRepo,
                                           const ServiceRepository&  serviceRepo)
  : memberRepo_(memberRepo), providerRepo_(providerRepo), serviceRepo_(serviceRepo) {}

// ── date helpers ──────────────────────────────────────────────────────────────

static bool parseDateMDY(const string& s, tm& out) {
  // MM-DD-YYYY
  if (s.size() != 10 || s[2] != '-' || s[5] != '-') return false;
  memset(&out, 0, sizeof(out));
  try {
    out.tm_mon  = stoi(s.substr(0, 2)) - 1;
    out.tm_mday = stoi(s.substr(3, 2));
    out.tm_year = stoi(s.substr(6, 4)) - 1900;
  } catch (...) { return false; }
  mktime(&out);
  return true;
}

static time_t toTime(const tm& t) {
  tm copy = t;
  return mktime(&copy);
}

bool WeeklyBatchGenerator::isInWindow_(const string& date, const tm& endTm) const {
  tm d;
  if (!parseDateMDY(date, d)) return false;
  time_t dt  = toTime(d);
  time_t end = toTime(endTm);
  time_t start = end - 6 * 86400;
  return dt >= start && dt <= end;
}

// ── main run ──────────────────────────────────────────────────────────────────

bool WeeklyBatchGenerator::run(const string& recordsPath,
                               const string& weekEndingDate,
                               const string& outputDir) const {
  tm endTm;
  if (!parseDateMDY(weekEndingDate, endTm)) return false;

  // Load records (safe — returns empty vector if file missing)
  ServiceRecordRepository repo(recordsPath);
  auto all = repo.loadAll();

  // Filter to window
  vector<ServiceRecord> recs;
  for (auto& r : all)
    if (isInWindow_(r.dateOfService, endTm)) recs.push_back(r);

  // Ensure output dir
  error_code ec;
  fs::create_directories(outputDir, ec);
  if (ec) return false;

  writeWeeklySummary_(recs, outputDir, weekEndingDate);
  writeProviderReports_(recs, outputDir);
  writeMemberReports_(recs, outputDir);
  writeAPSummary_(recs, outputDir);
  writeEFT_(recs, outputDir);
  return true;
}

// ── weekly summary ────────────────────────────────────────────────────────────

void WeeklyBatchGenerator::writeWeeklySummary_(const vector<ServiceRecord>& recs,
                                               const string& dir,
                                               const string& weekEndingDate) const {
  ofstream out(dir + "/weekly_summary.txt");
  out << "ChocAn Weekly Summary Report\n";
  out << "Week ending: " << weekEndingDate << "\n";
  out << string(50, '-') << "\n";

  if (recs.empty()) {
    out << "No records in this period.\n";
    return;
  }

  map<string, pair<int,double>> byProvider; // providerNumber → {count, total}
  double grandTotal = 0;
  for (auto& r : recs) {
    byProvider[r.providerNumber].first++;
    byProvider[r.providerNumber].second += r.fee;
    grandTotal += r.fee;
  }

  out << left << setw(15) << "Provider#"
      << setw(8)  << "Count"
      << "Total\n";
  out << string(50, '-') << "\n";
  for (auto& [num, p] : byProvider) {
    auto prov = providerRepo_.findByNumber(num);
    string name = prov ? prov->name : "(unknown)";
    out << left << setw(15) << num
        << setw(8)  << p.first
        << "$" << fixed << setprecision(2) << p.second
        << "  " << name << "\n";
  }
  out << string(50, '-') << "\n";
  out << "Total records: " << recs.size()
      << "   Grand total: $" << fixed << setprecision(2) << grandTotal << "\n";
}

// ── provider reports ──────────────────────────────────────────────────────────

void WeeklyBatchGenerator::writeProviderReports_(const vector<ServiceRecord>& recs,
                                                 const string& dir) const {
  // Group by provider
  map<string, vector<const ServiceRecord*>> byProv;
  for (auto& r : recs) byProv[r.providerNumber].push_back(&r);

  for (auto& [num, provRecs] : byProv) {
    auto prov = providerRepo_.findByNumber(num);
    string name = prov ? prov->name : "Unknown";

    ofstream out(dir + "/provider_" + num + ".txt");
    out << "ChocAn Provider Report\n";
    out << "Provider: " << name << " (" << num << ")\n";
    out << string(50, '-') << "\n";

    double total = 0;
    for (auto* r : provRecs) {
      auto svc = serviceRepo_.findByCode(r->serviceCode);
      string svcName = svc ? svc->name : r->serviceCode;
      auto mem = memberRepo_.findByNumber(r->memberNumber);
      string memName = mem ? mem->name : "(unknown)";

      out << "Date: "    << r->dateOfService
          << "  Member: " << memName << " (" << r->memberNumber << ")"
          << "  Service: " << svcName
          << "  Fee: $"  << fixed << setprecision(2) << r->fee << "\n";
      if (!r->comments.empty()) out << "  Comments: " << r->comments << "\n";
      total += r->fee;
    }
    out << string(50, '-') << "\n";
    out << "Total consultations: " << provRecs.size()
        << "   Total fee: $" << fixed << setprecision(2) << total << "\n";
  }
}

// ── member reports ────────────────────────────────────────────────────────────

void WeeklyBatchGenerator::writeMemberReports_(const vector<ServiceRecord>& recs,
                                               const string& dir) const {
  map<string, vector<const ServiceRecord*>> byMem;
  for (auto& r : recs) byMem[r.memberNumber].push_back(&r);

  for (auto& [num, memRecs] : byMem) {
    auto mem = memberRepo_.findByNumber(num);
    string name = mem ? mem->name : "Unknown";

    ofstream out(dir + "/member_" + num + ".txt");
    out << "ChocAn Member Report\n";
    out << "Member: " << name << " (" << num << ")\n";
    out << string(50, '-') << "\n";

    for (auto* r : memRecs) {
      auto svc = serviceRepo_.findByCode(r->serviceCode);
      string svcName = svc ? svc->name : r->serviceCode;
      auto prov = providerRepo_.findByNumber(r->providerNumber);
      string provName = prov ? prov->name : "(unknown)";

      out << "Date: "      << r->dateOfService
          << "  Provider: " << provName
          << "  Service: "  << svcName << "\n";
    }
    out << string(50, '-') << "\n";
    out << "Total services received: " << memRecs.size() << "\n";
  }
}

// ── AP summary ────────────────────────────────────────────────────────────────

void WeeklyBatchGenerator::writeAPSummary_(const vector<ServiceRecord>& recs,
                                           const string& dir) const {
  ofstream out(dir + "/ap_summary.txt");
  out << "ChocAn Accounts Payable Summary\n";
  out << string(50, '-') << "\n";

  map<string, double> byProv;
  for (auto& r : recs) byProv[r.providerNumber] += r.fee;

  double grand = 0;
  for (auto& [num, total] : byProv) {
    auto prov = providerRepo_.findByNumber(num);
    string name = prov ? prov->name : "(unknown)";
    out << left << setw(25) << name << setw(12) << num
        << "$" << fixed << setprecision(2) << total << "\n";
    grand += total;
  }
  out << string(50, '-') << "\n";
  out << "Total providers: " << byProv.size()
      << "   Total payable: $" << fixed << setprecision(2) << grand << "\n";
}

// ── EFT file ──────────────────────────────────────────────────────────────────

void WeeklyBatchGenerator::writeEFT_(const vector<ServiceRecord>& recs,
                                     const string& dir) const {
  ofstream out(dir + "/eft_output.txt");
  out << "ChocAn EFT Transfer File\n";
  out << string(50, '-') << "\n";

  map<string, double> byProv;
  for (auto& r : recs) byProv[r.providerNumber] += r.fee;

  for (auto& [num, total] : byProv) {
    auto prov = providerRepo_.findByNumber(num);
    string name = prov ? prov->name : "(unknown)";
    out << "TRANSFER  " << left << setw(25) << name
        << setw(12) << num
        << "$" << fixed << setprecision(2) << total << "\n";
  }
  out << string(50, '-') << "\n";
  out << "Records: " << byProv.size() << "\n";
}
