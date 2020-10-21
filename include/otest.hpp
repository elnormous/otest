#ifndef OTEST_HPP
#define OTEST_HPP

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace otest
{
    template <class ...T>
    class TestRunner final
    {
    private:
        template <class ...> friend int runTests(int, const char*[]);

        TestRunner() = default;

        int run(int argc, const char* argv[])
        {
            runTests<T...>();

            if (!failedTests)
                std::cout << "Success, tests run: " << testsRun << ", assertions: " << assertions << '\n';
            else
                std::cout << "Failure, tests run: " << testsRun << ", assertions: " << assertions << ", test failed: " << failedTests << '\n';

            return failedTests ? EXIT_FAILURE : EXIT_SUCCESS;
        }

        template <class T0>
        void runTests()
        {
            T0 t;

            testsRun += t.testsRun;
            failedTests += t.failedTests;
            assertions += t.assertions;
        }

        template <class T0, class T1, class ...Tn>
        void runTests()
        {
            T0 t;

            runTests<T1, Tn...>();

            testsRun += t.testsRun;
            failedTests += t.failedTests;
            assertions += t.assertions;
        }

        std::size_t testsRun = 0;
        std::size_t failedTests = 0;
        std::size_t assertions = 0;
    };

    template <class ...T> int runTests(int argc, const char* argv[])
    {
        TestRunner<T...> runner;
        return runner.run(argc, argv);
    }

    template <class TestContainer>
    class TestCase
    {
        template <class ...> friend class TestRunner;
    protected:
        TestCase() = default;

        template <class Function>
        void run(const std::string& name, const Function& function)
        {
            run(name, "", function);
        }

        template <class Function>
        void run(const std::string& name, const std::string& tag, const Function& function)
        {
            currentTest = TestInstance{};
            ++testsRun;

            try
            {
                function();
                if (currentTest.errors)
                    throw TestError(name + " failed with " + std::to_string(currentTest.errors) + " errors");
            }
            catch (std::exception& e)
            {
                ++failedTests;
                std::cerr << e.what() << '\n';
            }
            catch (...)
            {
                ++failedTests;
            }
        }

        void expect(bool condition)
        {
            ++assertions;
            ++currentTest.assertions;

            if (!condition)
            {
                ++errors;
                ++currentTest.errors;
                throw TestError("Failed");
            }
        }

        void check(bool condition)
        {
            ++assertions;
            ++currentTest.assertions;
            if (!condition)
            {
                ++errors;
                ++currentTest.errors;
            }
        }

    private:
        struct TestInstance final
        {
            std::size_t errors = 0;
            std::size_t assertions = 0;
        };

        class TestError final: public std::logic_error
        {
        public:
            explicit TestError(const std::string& str): std::logic_error(str) {}
            explicit TestError(const char* str): std::logic_error(str) {}
        };

        TestInstance currentTest;
        std::size_t testsRun = 0;
        std::size_t failedTests = 0;
        std::size_t errors = 0;
        std::size_t assertions = 0;
    };
}

#endif
