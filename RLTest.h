
/*!
 * \file
 * \brief Test library
 * \details A JUnit-kind library for creating C++ unit tests
 * \author Roman Loginov
 * \version 1.0
 */

#include <cstdio>
#include <cassert>
#include <iostream>
#include <cstdlib>

/*!
 * \brief Debugger class.<br>
 * Provides iterface to introduce tester and to say result of it<br>
 * \warning May be rubbish printed in terminals without colour support
 */
class RLTestStatusDumper
{
private:
    bool* testStatus_; //!< Pointer to a status if test failed or not
    const char* testName_; //!< Test name, its' unique identifyer
    
    /*!
     * Provides in green light information that test passed successfully
     */
    void reportTestFailure()
    {
        printf("\033[1;31mTest failed!\033[0m\n");
    }
    
    /*!
     * In bold red indicates the test failure
     */
    void reportTestSuccess()
    {
        printf("\033[0;32mTest passed!\033[0m\n");
    }
    
    /*!
     * Introductory test info
     */
    void printGreetings()
    {
        printf("=============================================\n"
               "Running test: %s\n", testName_);
    }
    
    /*!
     * Checks test status and reports about it
     */
    void reportStatus()
    {
        if (*testStatus_)
            reportTestSuccess();
        else
        {
            reportTestFailure();
            printf("Please see the reason upper\n");
        }
        
        printf("=============================================\n");
    }

public:
    /*!
     * \brief Constructor.
     * \details Initializes test and says hello to user.
     * @param testStatus Pointer to a status variable, defined in a test function
     * @param testName Name of a test. Its' identifyer
     * @see #DEFINE_TEST(NAME)
     */
    RLTestStatusDumper(bool* testStatus, const char* testName):
        testStatus_(testStatus),
        testName_(testName)
    {
        assert(testStatus);
        printGreetings();
    }
    
    /*!
     * \brief Destructor.
     * \details Summarizes information about test and prints it
     */
    ~RLTestStatusDumper()
    {
        reportStatus();
    }
};

/*!
 * Test definition macro.
 * It initializes the dumper and then gives freedom to user.<br>
 * Another macro make easier usage of tests.
 * @param NAME Name of your new test<br>
 */
#define DEFINE_TEST(NAME)                                           \
bool RLTest_##NAME()                                                \
{                                                                   \
    bool testStatus = true;                                         \
    RLTestStatusDumper dumper(&testStatus, #NAME);                                                               

/*!
 * Checks if COND is true
 * @param COND - condition to check
 */
#define ASSERT_TRUE(COND)                                           \
if (!(COND))                                                          \
{                                                                   \
    printf("Test failure: "#COND" is not true\n");                    \
    testStatus = false;                                             \
    return 0;                                                       \
}

/*!
 * Crashes the test with specified REASON
 * @param REASON - reason of crash
 */
#define CRASH_TEST(REASON)                                          \
printf("Test failure: %s\n", REASON);                               \
testStatus = false;                                                 \
return 0;                                              

/*!
 * Checks if VAR1 and VAR2 are equal<br>
 * Dumps expression for debug
 * @param VAR1, VAR2 - expressions to compare
 * @see #ASSERT_CLOSE(VAR1, VAR2)
 */
#define ASSERT_EQUAL(VAR1, VAR2)                                    \
if (VAR1 != VAR2)                                                   \
{                                                                   \
    printf("Variables should be equal,"                             \
           "but they are not\n"                                     \
           #VAR1": ");                                              \
    std::cout << VAR1 << "\n";                                      \
    printf(#VAR2": ");                                              \
    std::cout << VAR2 << "\n";                                      \
    testStatus = false;                                             \
    return 0;                                                       \
}

/*!
 * Checks if VAR1 and VAR2 are practically equal<br>
 * Dumps expression for debug
 * @param VAR1, VAR2 - expressions to compare
 * @see #ASSERT_EQUAL(VAR1, VAR2)
 */
#define ASSERT_CLOSE(VAR1, VAR2)                                    \
if (fabs(VAR1 - VAR2) > 1e-5)                                       \
{                                                                   \
    printf("Variables should be close,"                             \
           "but they are not\n"                                     \
           #VAR1": ");                                              \
    std::cout << VAR1 << "\n";                                      \
    printf(#VAR2": ");                                              \
    std::cout << VAR2 << "\n";                                      \
    testStatus = false;                                             \
    return 0;                                                       \
}

#ifndef SKIPPED_BY_DOXYGEN

#define CODE_CAUSES_ASSERTION(CODE)                                 \
auto previousHandler = std::signal(SIGABRT, abortIndicatorHandler); \
if (previousHandler == SIG_ERR)                                     \
{                                                                   \
    printf("Handler of abort remained standart\n");                 \
}                                                                   \
                                                                    \
__TEST_ABORT_FLAG__ = false;                                        \
{  CODE  }                                                          \
                                                                    \
returnedHandler = std::signal(SIGABRT, previousHandler);            \
if (returnedHandler == SIG_ERR)                                     \
{                                                                   \
    printf("Unable to return old handler\n");                       \
    abort();                                                        \
}                                                                   \
                                                                    \
if (!__TEST_ABORT_FLAG__)                                           \
{                                                                   \
    testStatus = false;                                             \
    printf("Test failure: error is not handled\n");                 \
    return 0;                                                       \
}                                                                   

#endif /* SKIPPED_BY_DOXYGEN */

/*!
 * Runs test identified by name
 * @param NAME - Name of a test
 */
#define RUN_TEST(NAME) RLTest_##NAME()

