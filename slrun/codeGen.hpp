#ifndef SL_CODEGEN_HPP
#define SL_CODEGEN_HPP

#include <vector>
#include <sstream>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/array.hpp>

#include "st.hpp"

namespace sl
{

struct BP
{
    int off;

    explicit BP(int off) : off(off) { }
};

struct BPAddr
{
    int off;

    explicit BPAddr(int off) : off(off) { }
};

struct BPDeref
{
    int off;

    explicit BPDeref(int off) : off(off) { }
};

struct DataAddr
{
    unsigned off;

    explicit DataAddr(unsigned off) : off(off) { }
};

struct CodeAddr
{
    unsigned off;

    explicit CodeAddr(unsigned off) : off(off) { }
};

inline bool operator==(const CodeAddr& left, const CodeAddr& right)
{
    return left.off == right.off;
}

inline bool operator<(const CodeAddr& left, const CodeAddr& right)
{
    return left.off < right.off;
}

namespace opcodes
{

enum Tag
{
    MOV,
    
    ADD, SUB, MUL, DIV, MOD,
    
    CALL, ENTER, LEAVE, RET,
    
    PUSH, INCSP,
    
    EXIT,

    INTTOREAL, REALTOINT
};

enum Type
{
    INT,
    REAL
};

class Opcode
{
public:

    typedef boost::variant<DataAddr, CodeAddr, BP, BPAddr, BPDeref, int, float>  ArgType;

    Opcode(Tag tag) : tag_(tag) { }

    Opcode(Tag tag, Type type) : tag_(tag), type_(type) { }

    Opcode(Tag tag, ArgType arg0) : tag_(tag)
    {
        args_[0] = arg0;
    }

    Opcode(Tag tag, Type type, ArgType arg0) : tag_(tag), type_(type)
    {
        args_[0] = arg0;
    }

    Opcode(Tag tag, ArgType arg0, ArgType arg1) : tag_(tag)
    {
        args_[0] = arg0;
        args_[1] = arg1;
    }

    Opcode(Tag tag, Type type, ArgType arg0, ArgType arg1) : tag_(tag), type_(type)
    {
        args_[0] = arg0;
        args_[1] = arg1;
    }

    Opcode(Tag tag, ArgType arg0, ArgType arg1, ArgType arg2) : tag_(tag)
    {
        args_[0] = arg0;
        args_[1] = arg1;
        args_[2] = arg2;
    }

    Tag tag() const { return tag_; }

    boost::optional<Type> type() const { return type_; }

    const boost::optional<ArgType>& arg(unsigned i) const
    {
        return args_[i];
    }

private:

    Tag tag_;
    boost::optional<Type> type_;
    boost::array<boost::optional<ArgType>, 3> args_;
};

}

class BytecodeBuffer
{
public:
   
    BytecodeBuffer() { }

    BytecodeBuffer(BytecodeBuffer&& right)
    {
        code_.swap(right.code_);
    }

    CodeAddr append(opcodes::Opcode oc)
    {
        code_.push_back(oc);

        return CodeAddr(code_.size() - 1);
    }

    opcodes::Opcode& operator[](CodeAddr addr)
    {
        return code_[addr.off];
    }

    const opcodes::Opcode& operator[](CodeAddr addr) const
    {
        return code_[addr.off];
    }

    unsigned size() const { return code_.size(); }

private:

    std::vector<opcodes::Opcode> code_;
};


BytecodeBuffer generateBytecode(const st::Module& module);

void exportToAsm(const BytecodeBuffer& bb, std::ostream& os);

}

#endif /* SL_CODEGEN_HPP */