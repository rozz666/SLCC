#ifndef TUT_CONSOLE_REPORTER
#define TUT_CONSOLE_REPORTER

#include <Windows.h>
#include <tut/tut.hpp>
#include <cassert>

namespace
{

class console
{
    HANDLE handle;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

public:

    console()
    {
        handle = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(handle, &csbi);
    }

    void reset_color()
    {
        SetConsoleTextAttribute(handle, csbi.wAttributes);
    }

    void set_color(WORD color)
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi = this->csbi;

        csbi.wAttributes &= (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
        csbi.wAttributes |= color & (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

        SetConsoleTextAttribute(handle, csbi.wAttributes);
    }

} console;

std::ostream& cr(std::ostream& os)
{
    os.flush();
    console.reset_color();
    return os;
}

std::ostream& green(std::ostream& os)
{
    os.flush();
    console.set_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& red(std::ostream& os)
{
    os.flush();
    console.set_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& operator<<(std::ostream& os, const tut::test_result& tr)
{
    switch(tr.result)
    {
    case tut::test_result::ok:
        os << green << "." << cr;
        break;
    case tut::test_result::fail:
        os << red << '[' << tr.test << "=F]" << cr;
        break;
    case tut::test_result::ex_ctor:
        os << red << '[' << tr.test << "=C]" << cr;
        break;
    case tut::test_result::ex:
        os << red << '[' << tr.test << "=X]" << cr;
        break;
    case tut::test_result::warn:
        os << red << '[' << tr.test << "=W]" << cr;
        break;
    case tut::test_result::term:
        os << red << '[' << tr.test << "=T]" << cr;
        break;
    case tut::test_result::rethrown:
        os << red << '[' << tr.test << "=P]" << cr;
        break;
    case tut::test_result::dummy:
        assert(!"Should never be called");
    }

    return os;
}

} // end of namespace

namespace tut
{

/**
 * Default TUT callback handler.
 */
class color_console_reporter : public tut::callback
{
    std::string current_group;
    typedef std::vector<tut::test_result> not_passed_list;
    not_passed_list not_passed;
    std::ostream& os;

public:

    int ok_count;
    int exceptions_count;
    int failures_count;
    int terminations_count;
    int warnings_count;

    color_console_reporter()
        : os(std::cout)
    {
        init();
    }

    void run_started()
    {
        init();
    }

    void test_completed(const tut::test_result& tr)
    {
        if (tr.group != current_group)
        {
            os << std::endl << tr.group << ":" << std::flush;
            current_group = tr.group;
        }

        os << tr << std::flush;

        // update global statistics
        switch (tr.result) {
            case test_result::ok:
                ok_count++;
                break;
            case test_result::fail:
            case test_result::rethrown:
                failures_count++;
                break;
            case test_result::ex:
            case test_result::ex_ctor:
                exceptions_count++;
                break;
            case test_result::warn:
                warnings_count++;
                break;
            case test_result::term:
                terminations_count++;
                break;
            case tut::test_result::dummy:
                assert(!"Should never be called");
        } // switch

        if (tr.result != tut::test_result::ok)
        {
            not_passed.push_back(tr);
        }
    }

    void run_completed()
    {
        os << std::endl;

        if (not_passed.size() > 0)
        {
            os << std::endl << "Failed tests:" << std::endl;

            not_passed_list::const_iterator i = not_passed.begin();
            while (i != not_passed.end())
            {
                tut::test_result tr = *i;

                os << std::endl;

                os << "   group: " << tr.group
                << ", test: test<" << tr.test << ">"
                << (!tr.name.empty() ? (std::string(" : ") + tr.name) : std::string()) << cr
                << std::endl;

#if defined(TUT_USE_POSIX)
                if(tr.pid != getpid())
                {
                    os << "   child pid: " << tr.pid << std::endl;
                }
#endif
                os << "   problem: " << red;
                switch(tr.result)
                {
                case test_result::rethrown:
                    os << "assertion failed in child" << std::endl;
                    break;
                case test_result::fail:
                    os << "assertion failed" << std::endl;
                    break;
                case test_result::ex:
                case test_result::ex_ctor:
                    os << "unexpected exception" << std::endl;
                    if( tr.exception_typeid != "" )
                    {
                        os << "     exception typeid: "
                        << tr.exception_typeid << std::endl;
                    }
                    break;
                case test_result::term:
                    os << "would be terminated" << std::endl;
                    break;
                case test_result::warn:
                    os << "test passed, but cleanup code (destructor) raised"
                        " an exception" << std::endl;
                    break;
                default:
                    break;
                }

                os << cr;

                if (!tr.message.empty())
                {
                    if (tr.result == test_result::fail)
                    {
                        os << "   failed assertion: " << red << tr.message << cr
                            << std::endl;
                    }
                    else
                    {
                        os << "   message: " << red << tr.message << cr
                            << std::endl;
                    }
                }

                ++i;
            }
        }

        os << std::endl;

        os << "tests summary:";
        if (terminations_count > 0)
        {
            os << " terminations:" << terminations_count;
        }
        if (exceptions_count > 0)
        {
            os << " exceptions:" << exceptions_count;
        }
        if (failures_count > 0)
        {
            os << " failures:" << failures_count;
        }
        if (warnings_count > 0)
        {
            os << " warnings:" << warnings_count;
        }
        os << " ok:" << ok_count;
        os << std::endl;
    }

    bool all_ok() const
    {
        return not_passed.empty();
    }

private:

    void init()
    {
        ok_count = 0;
        exceptions_count = 0;
        failures_count = 0;
        terminations_count = 0;
        warnings_count = 0;
        not_passed.clear();
    }
};

}

#endif
