#include "TestFramework.hpp"
#include "../include/MacAddress.hpp"

#include <stdexcept>

// ──── Construction et validation ────

TEST(MacAddress_validColon) {
    myzone::MacAddress mac("AA:BB:CC:DD:EE:FF");
    ASSERT_TRUE(mac.isValid());
    ASSERT_EQ(mac.toString(), "AA:BB:CC:DD:EE:FF");
    ASSERT_EQ(mac.compact(), "AABBCCDDEEFF");
}

TEST(MacAddress_validDash) {
    myzone::MacAddress mac("11-22-33-44-55-66");
    ASSERT_TRUE(mac.isValid());
    ASSERT_EQ(mac.toString(), "11:22:33:44:55:66");
    ASSERT_EQ(mac.compact(), "112233445566");
}

TEST(MacAddress_validCompact) {
    myzone::MacAddress mac("aabbccddeeff");
    ASSERT_TRUE(mac.isValid());
    ASSERT_EQ(mac.toString(), "AA:BB:CC:DD:EE:FF");
    ASSERT_EQ(mac.compact(), "AABBCCDDEEFF");
}

TEST(MacAddress_validLowerCase) {
    myzone::MacAddress mac("aa:bb:cc:dd:ee:ff");
    ASSERT_TRUE(mac.isValid());
    ASSERT_EQ(mac.compact(), "AABBCCDDEEFF");
}

TEST(MacAddress_validMixedCase) {
    myzone::MacAddress mac("Aa:Bb:Cc:Dd:Ee:Ff");
    ASSERT_TRUE(mac.isValid());
    ASSERT_EQ(mac.compact(), "AABBCCDDEEFF");
}

// ──── Formats invalides ────

TEST(MacAddress_invalidEmpty) {
    ASSERT_THROWS(myzone::MacAddress(""), std::invalid_argument);
}

TEST(MacAddress_invalidTooShort) {
    ASSERT_THROWS(myzone::MacAddress("AA:BB:CC"), std::invalid_argument);
}

TEST(MacAddress_invalidTooLong) {
    ASSERT_THROWS(myzone::MacAddress("AA:BB:CC:DD:EE:FF:00"), std::invalid_argument);
}

TEST(MacAddress_invalidCharacters) {
    ASSERT_THROWS(myzone::MacAddress("GG:HH:II:JJ:KK:LL"), std::invalid_argument);
}

TEST(MacAddress_invalidMixedSeparators) {
    ASSERT_THROWS(myzone::MacAddress("AA:BB-CC:DD-EE:FF"), std::invalid_argument);
}

TEST(MacAddress_invalidSpaces) {
    ASSERT_THROWS(myzone::MacAddress("AA BB CC DD EE FF"), std::invalid_argument);
}

// ──── Default constructor ────

TEST(MacAddress_defaultConstructor) {
    myzone::MacAddress mac;
    ASSERT_FALSE(mac.isValid());
    ASSERT_EQ(mac.compact(), "");
    ASSERT_EQ(mac.toString(), "INVALID_MAC");
}

// ──── Edge cases ────

TEST(MacAddress_allZeros) {
    myzone::MacAddress mac("00:00:00:00:00:00");
    ASSERT_TRUE(mac.isValid());
    ASSERT_EQ(mac.compact(), "000000000000");
}

TEST(MacAddress_allF) {
    myzone::MacAddress mac("FF:FF:FF:FF:FF:FF");
    ASSERT_TRUE(mac.isValid());
    ASSERT_EQ(mac.compact(), "FFFFFFFFFFFF");
}

TEST(MacAddress_leadingTrailingSpaces) {
    myzone::MacAddress mac("  AA:BB:CC:DD:EE:FF  ");
    ASSERT_TRUE(mac.isValid());
    ASSERT_EQ(mac.compact(), "AABBCCDDEEFF");
}
