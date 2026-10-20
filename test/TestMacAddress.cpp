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

// ──── Propriétés IEEE 802 (LAA, Multicast, Broadcast, Unicast) ────

TEST(MacAddress_ieeeProperties_OfficialUnicast) {
    // 00:1A:2B:33:44:55 -> 00 en binaire: 0000 0000 -> bit 0=0 (unicast), bit 1=0 (universel)
    myzone::MacAddress mac("00:1A:2B:33:44:55");
    ASSERT_TRUE(mac.isValid());
    ASSERT_TRUE(mac.isUniversallyAdministered());
    ASSERT_FALSE(mac.isLocallyAdministered());
    ASSERT_TRUE(mac.isUnicast());
    ASSERT_FALSE(mac.isMulticast());
    ASSERT_FALSE(mac.isBroadcast());
    ASSERT_FALSE(mac.isZero());
}

TEST(MacAddress_ieeeProperties_RandomizedLAA) {
    // DA:A1:19:12:34:56 -> DA en binaire: 1101 1010 -> bit 0=0 (unicast), bit 1=1 (LAA / aléatoire privée)
    myzone::MacAddress mac("DA:A1:19:12:34:56");
    ASSERT_TRUE(mac.isValid());
    ASSERT_TRUE(mac.isLocallyAdministered());
    ASSERT_FALSE(mac.isUniversallyAdministered());
    ASSERT_TRUE(mac.isUnicast());
    ASSERT_FALSE(mac.isMulticast());
}

TEST(MacAddress_ieeeProperties_Multicast) {
    // 01:00:5E:00:00:01 (IPv4 Multicast) -> 01 en binaire: 0000 0001 -> bit 0=1 (multicast)
    myzone::MacAddress mac("01:00:5E:00:00:01");
    ASSERT_TRUE(mac.isValid());
    ASSERT_TRUE(mac.isMulticast());
    ASSERT_FALSE(mac.isUnicast());
    ASSERT_FALSE(mac.isBroadcast());
}

TEST(MacAddress_ieeeProperties_Broadcast) {
    myzone::MacAddress mac("FF:FF:FF:FF:FF:FF");
    ASSERT_TRUE(mac.isValid());
    ASSERT_TRUE(mac.isBroadcast());
    ASSERT_TRUE(mac.isMulticast()); // Broadcast est un type de multicast
    ASSERT_TRUE(mac.isLocallyAdministered());
    ASSERT_FALSE(mac.isZero());
}

TEST(MacAddress_ieeeProperties_Zero) {
    myzone::MacAddress mac("00:00:00:00:00:00");
    ASSERT_TRUE(mac.isValid());
    ASSERT_TRUE(mac.isZero());
    ASSERT_TRUE(mac.isUnicast());
    ASSERT_TRUE(mac.isUniversallyAdministered());
    ASSERT_FALSE(mac.isBroadcast());
}
