#include "TestFramework.hpp"
#include "../include/MacAddress.hpp"
#include <unordered_set>
#include <optional>

// Test operator==
TEST(MacAddress_equalSame) {
    myzone::MacAddress mac1("00:11:22:33:44:55");
    myzone::MacAddress mac2("00:11:22:33:44:55");
    ASSERT_TRUE(mac1 == mac2);
}

TEST(MacAddress_equalDifferentFormat) {
    myzone::MacAddress mac1("00:11:22:33:44:55");
    myzone::MacAddress mac2("00-11-22-33-44-55");
    ASSERT_TRUE(mac1 == mac2);
}

TEST(MacAddress_notEqualDifferent) {
    myzone::MacAddress mac1("00:11:22:33:44:55");
    myzone::MacAddress mac2("AA:BB:CC:DD:EE:FF");
    ASSERT_FALSE(mac1 == mac2);
}

TEST(MacAddress_equalBothInvalid) {
    myzone::MacAddress mac1;
    myzone::MacAddress mac2;
    ASSERT_TRUE(mac1 == mac2);
}

TEST(MacAddress_notEqualValidInvalid) {
    myzone::MacAddress mac1("00:11:22:33:44:55");
    myzone::MacAddress mac2;
    ASSERT_FALSE(mac1 == mac2);
}

// Test operator<
TEST(MacAddress_lessThan) {
    myzone::MacAddress mac1("00:11:22:33:44:55");
    myzone::MacAddress mac2("00:11:22:33:44:56");
    ASSERT_TRUE(mac1 < mac2);
    ASSERT_FALSE(mac2 < mac1);
}

TEST(MacAddress_lessThanInvalid) {
    myzone::MacAddress mac1; // invalid
    myzone::MacAddress mac2("00:11:22:33:44:55");
    ASSERT_TRUE(mac1 < mac2);
    ASSERT_FALSE(mac2 < mac1);
}

// Test std::hash
TEST(MacAddress_hashConsistent) {
    myzone::MacAddress mac1("00:11:22:33:44:55");
    myzone::MacAddress mac2("00-11-22-33-44-55");
    std::hash<myzone::MacAddress> hasher;
    ASSERT_INT_EQ(hasher(mac1), hasher(mac2));
}

TEST(MacAddress_hashInUnorderedSet) {
    std::unordered_set<myzone::MacAddress> s;
    myzone::MacAddress mac1("00:11:22:33:44:55");
    myzone::MacAddress mac2("00-11-22-33-44-55");
    s.insert(mac1);
    ASSERT_TRUE(s.find(mac2) != s.end());
}

// Test tryParse
TEST(MacAddress_tryParseValid) {
    auto res = myzone::MacAddress::tryParse("00:11:22:33:44:55");
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res->toString(), "00:11:22:33:44:55");
}

TEST(MacAddress_tryParseInvalid) {
    auto res = myzone::MacAddress::tryParse("not-a-mac");
    ASSERT_FALSE(res.has_value());
}

TEST(MacAddress_tryParseEmpty) {
    auto res = myzone::MacAddress::tryParse("");
    ASSERT_FALSE(res.has_value());
}
