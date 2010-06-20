#ifndef SL_TEST_VALUELIST_HPP
#define SL_TEST_VALUELIST_HPP

#include <sl/def.hpp>
#include <sl/test/ValueStream.hpp>

namespace sl
{
namespace test
{

class ValueList
{
public:

    class ValueListData
    {
    public:

        ValueListData(int value)
        {
            vs_.put(sl::int_t(value));
        }

        ValueListData(double value)
        {
            vs_.put(sl::float_t(value));
        }

        operator ValueStream& ()
        {
            return vs_;
        }

        ValueListData& operator,(int value)
        {
            vs_.put(sl::int_t(value));
            return *this;
        }

        ValueListData& operator,(double value)
        {
            vs_.put(sl::float_t(value));
            return *this;
        }

    private:

        ValueStream vs_;
    };

    ValueListData operator,(int value) const
    {
        return ValueListData(value);
    }

    ValueListData operator,(double value) const
    {
        return ValueListData(value);
    }

};

}
}

#endif /* SL_TEST_VALUELIST_HPP */