#ifndef SL_VM_STREAMIO_HPP
#define SL_VM_STREAMIO_HPP

namespace sl
{
namespace vm
{

class StreamIO
{
public:

    virtual ~StreamIO() { }

    virtual void putInt(int_t val) = 0;
    virtual void putFloat(float_t val) = 0;
    virtual int_t getInt() = 0;
    virtual float_t getFloat() = 0;
};

}
}

#endif /* SL_VM_STREAMIO_HPP */