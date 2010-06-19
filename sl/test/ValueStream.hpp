#ifndef SL_TEST_VALUESTREAM_HPP
#define SL_TEST_VALUESTREAM_HPP

#include <deque>
#include <ostream>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/variant.hpp>
#include <sl/def.hpp>

namespace sl
{
namespace test
{

class ValueStream : boost::noncopyable
{
public:

    template <typename T>
    void put(T val)
    {
        queue_.push_back(val);
    }

    template <typename T>
    boost::optional<T> get()
    {
        if (queue_.empty()) return boost::none;

        if (queue_.front().type() != typeid(T)) return boost::none;
            
        T val = boost::get<T>(queue_.front());
        queue_.pop_front();
        return val;
    }

    friend inline bool operator==(const ValueStream& left, const ValueStream& right)
    {
        return left.queue_ == right.queue_;
    }

    friend inline std::ostream& operator<<(std::ostream& left, const ValueStream& right)
    {
        auto it = right.queue_.begin();

        if (it != right.queue_.end())
        {
            left << *it;
            ++it;

            for (; it != right.queue_.end(); ++it)
            {
                left << " " << *it;
            }
        }

        return left;
    }

private:

    std::deque<boost::variant<int_t, float_t> > queue_;
};

inline bool operator!=(const ValueStream& left, const ValueStream& right)
{
    return !(left == right);
}

}
}

#endif /* SL_TEST_VALUESTREAM_HPP */