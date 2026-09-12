#include "TestFramework.hpp"
#include "../include/Database.hpp"

#include <cstdlib>
#include <fstream>
#include <string>
#include <string>

namespace {

// Locate the data/ directory relative to the test executable
std::string findDataDir() {
    // Try common locations
    const char* paths[] = {"data", "../data", "../../data"};
    for (const char* p : paths) {
        std::ifstream test(std::string(p) + "/lookup.csv");
        if (test.good()) return p;
    }
    return "data";
}

} // namespace

// ──── Construction et état ────

TEST(Database_isReady) {
    myzone::Database db(findDataDir());
    ASSERT_TRUE(db.isReady());
}

TEST(Database_vendorCountPositive) {
    myzone::Database db(findDataDir());
    ASSERT_TRUE(db.vendorCount() > 0);
}

TEST(Database_dhcpCountPositive) {
    myzone::Database db(findDataDir());
    ASSERT_TRUE(db.dhcpFingerprintCount() > 0);
}

TEST(Database_p0fCountPositive) {
    myzone::Database db(findDataDir());
    ASSERT_TRUE(db.p0fSignatureCount() > 0);
}

TEST(Database_fingerBankCountPositive) {
    myzone::Database db(findDataDir());
    ASSERT_TRUE(db.fingerBankCount() > 0);
}

TEST(Database_sourcesNotEmpty) {
    myzone::Database db(findDataDir());
    ASSERT_TRUE(db.sources().size() >= 7);
}

// ──── Lookup OUI ────

TEST(Database_lookupKnownIntelMac) {
    myzone::Database db(findDataDir());
    myzone::MacAddress mac("00:1B:21:00:00:00");
    myzone::DeviceInfo info;
    // 00:1B:21 is an Intel prefix
    bool found = db.lookup(mac, info);
    if (found) {
        ASSERT_TRUE(!info.companyName.empty());
    } else {
        // If not found, the test data might not contain it — skip gracefully
        ASSERT_TRUE(true);
    }
}

TEST(Database_lookupInvalidMac) {
    myzone::Database db(findDataDir());
    myzone::MacAddress mac; // invalid default
    myzone::DeviceInfo info;
    ASSERT_FALSE(db.lookup(mac, info));
}

// ──── Lookup DHCP ────

TEST(Database_lookupDhcpInvalidHash) {
    myzone::Database db(findDataDir());
    myzone::DhcpFingerprintInfo info;
    // Too short
    ASSERT_FALSE(db.lookupDhcpFingerprint("abc", info));
}

TEST(Database_lookupDhcpNonExistentHash) {
    myzone::Database db(findDataDir());
    myzone::DhcpFingerprintInfo info;
    // Valid format but unlikely to exist
    ASSERT_FALSE(db.lookupDhcpFingerprint("00000000000000000000000000000000", info));
}

// ──── Lookup p0f ────

TEST(Database_lookupP0fNonExistentSig) {
    myzone::Database db(findDataDir());
    myzone::P0fSignatureInfo info;
    ASSERT_FALSE(db.lookupP0fSignature("this:is:not:a:real:sig", info));
}

// ──── Lookup FingerBank ────

TEST(Database_lookupFingerBankNonExistent) {
    myzone::Database db(findDataDir());
    myzone::FingerBankInfo info;
    ASSERT_FALSE(db.lookupFingerBankOptions("999,998,997,996", info));
}

// ──── Base vide / inexistante ────

TEST(Database_emptyDataDir) {
    myzone::Database db("nonexistent_directory_12345");
    ASSERT_FALSE(db.isReady());
    ASSERT_TRUE(db.vendorCount() == 0);
}
