#ifndef SL_TEST_VALUESTREAM_HPP
#define SL_TEST_VALUESTREAM_HPP

#include <queue>
#include <boost/optional.hpp>
#include <boost/any.hpp>

namespace sl
{
namespace test
{

class ValueStream
{
public:

    template <typename T>
    void put(T val)
    {
        queue_.push(val);
    }

    template <typename T>
    boost::optional<T> get()
    {
        if (queue_.empty()) return boost::none;

        T *p = boost::any_cast<T>(&queue_.front());

        if (!p) return boost::none;

        T val = *p;
        queue_.pop();
        return val;
    }

private:

    std::queue<boost::any> queue_;
};

}
}

#endif /* SL_TEST_VALUESTREAM_HPP */