#ifndef MYZONE_TEST_FRAMEWORK_HPP
#define MYZONE_TEST_FRAMEWORK_HPP

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace myzone {
namespace test {

struct TestCase {
    std::string name;
    std::function<void()> run;
};

inline std::vector<TestCase>& testRegistry() {
    static std::vector<TestCase> tests;
    return tests;
}

inline int& failCount() {
    static int count = 0;
    return count;
}

inline int& passCount() {
    static int count = 0;
    return count;
}

struct TestRegistrar {
    TestRegistrar(const std::string& name, std::function<void()> fn) {
        testRegistry().push_back({name, std::move(fn)});
    }
};

inline void assertImpl(bool condition,
                       const std::string& expr,
                       const std::string& file,
                       int line) {
    if (condition) {
        ++passCount();
    } else {
        ++failCount();
        std::cerr << "  ECHEC  " << file << ":" << line
                  << "  ->  " << expr << std::endl;
    }
}

inline void assertEqualImpl(const std::string& actual,
                            const std::string& expected,
                            const std::string& exprActual,
                            const std::string& exprExpected,
                            const std::string& file,
                            int line) {
    if (actual == expected) {
        ++passCount();
    } else {
        ++failCount();
        std::cerr << "  ECHEC  " << file << ":" << line
                  << "  ->  " << exprActual << " == " << exprExpected
                  << "  (obtenu: \"" << actual << "\", attendu: \"" << expected << "\")"
                  << std::endl;
    }
}

inline int runAllTests() {
    passCount() = 0;
    failCount() = 0;

    std::cout << "\n====== Tests MyZone ======\n\n";

    for (const auto& test : testRegistry()) {
        std::cout << "  ▸ " << test.name << " ... " << std::flush;
        int failsBefore = failCount();
        try {
            test.run();
        } catch (const std::exception& e) {
            ++failCount();
            std::cerr << "\n  EXCEPTION  " << e.what() << std::endl;
        } catch (...) {
            ++failCount();
            std::cerr << "\n  EXCEPTION INCONNUE" << std::endl;
        }
        if (failCount() == failsBefore) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "ECHEC" << std::endl;
        }
    }

    std::cout << "\n====== Résultats ======\n";
    std::cout << "  " << passCount() << " assertions réussies, "
              << failCount() << " échouées\n\n";

    return failCount() == 0 ? 0 : 1;
}

} // namespace test
} // namespace myzone

#define TEST(name) \
    static void test_##name(); \
    static myzone::test::TestRegistrar registrar_##name(#name, test_##name); \
    static void test_##name()

#define ASSERT_TRUE(expr) \
    myzone::test::assertImpl((expr), #expr, __FILE__, __LINE__)

#define ASSERT_FALSE(expr) \
    myzone::test::assertImpl(!(expr), "!(" #expr ")", __FILE__, __LINE__)

#define ASSERT_EQ(actual, expected) \
    myzone::test::assertEqualImpl( \
        std::string(actual), std::string(expected), \
        #actual, #expected, __FILE__, __LINE__)

#define ASSERT_THROWS(expr, exType) \
    do { \
        bool caught = false; \
        try { expr; } catch (const exType&) { caught = true; } \
        myzone::test::assertImpl(caught, #expr " throws " #exType, __FILE__, __LINE__); \
    } while (false)

#define ASSERT_NO_THROW(expr) \
    do { \
        bool caught = false; \
        try { expr; } catch (...) { caught = true; } \
        myzone::test::assertImpl(!caught, #expr " no throw", __FILE__, __LINE__); \
    } while (false)

#endif // MYZONE_TEST_FRAMEWORK_HPP
