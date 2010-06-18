#ifndef SL_TEST_VALUESTREAMIO_HPP
#define SL_TEST_VALUESTREAMIO_HPP

#include <sl/def.hpp>
#include <sl/test/ValueStream.hpp>
#include <sl/vm/StreamIO.hpp>

namespace sl
{
namespace test
{

class ValueStreamIO : public vm::StreamIO
{
public:

    ValueStreamIO(ValueStream& is, ValueStream& os) : is_(&is), os_(&os) { }

    virtual void putInt(int_t val) { os_->put(val); }
    virtual void putFloat(float_t val) { os_->put(val); }
    virtual int_t getInt() { return *is_->get<int_t>();  }
    virtual float_t getFloat() { return *is_->get<float_t>(); }

private:

    ValueStream *is_;
    ValueStream *os_;
};

}
}

#endif /* SL_TEST_VALUESTREAMIO_HPP */