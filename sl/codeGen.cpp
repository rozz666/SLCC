#include <boost/assign/list_of.hpp>
#include <boost/function.hpp>
#include <sl/codeGen.hpp>
#include <sl/builtin.hpp>
#include <sl/DependencyGraph.hpp>
#include <sl/err/messages.hpp>
#include <sl/vm/opcodes.hpp>
#include <sl/vm/CodeGenerator.hpp>

namespace sl
{

BasicType expressionType(const ast::Expression& expr);
FilePosition expressionPos(const ast::Expression& expr);

namespace
{

class OpcodePrinter
{
public:

    OpcodePrinter() : name_("XXX") { }

    OpcodePrinter(const std::string& name) : name_(name), argSize_(0), useLabels_(false) { }
    OpcodePrinter(const std::string& name, unsigned argSize, bool useLabels = false)
        : name_(name), argSize_(argSize), useLabels_(useLabels) { }

    template <typename It>
    It operator()(It it, std::ostream& os, const std::vector<std::string>& labels) const
    {
        os << name_;
        ++it;

        if (argSize_)
        {
            std::int32_t val = *it++;

            if (argSize_ > 1)
            {
                val |= *it++ << 8;

                if (argSize_ > 2)
                {
                    val |= *it++ << 16;
                    val |= *it++ << 24;
                }
                else
                {
                    val = std::int16_t(val);
                }
            }

            os << " ";

            if (useLabels_ && val >= 0 && val < std::int32_t(labels.size()) && !labels[val].empty()) os << labels[val];
            else os << val;
        }

        return it;
    }

    vm::CodeAddr size() const
    {
        return argSize_ + 1;
    }

private:

    std::string name_;
    unsigned argSize_;
    bool useLabels_;
};

std::map<std::uint8_t, OpcodePrinter> opcodePrinters = boost::assign::map_list_of
    (vm::NOP, OpcodePrinter("nop"))
    (vm::CONST4, OpcodePrinter("const4", 4, true))
    (vm::LADDR, OpcodePrinter("laddr", 2))
    (vm::LOAD4, OpcodePrinter("load4"))
    (vm::LLOAD4, OpcodePrinter("lload4", 2))
    (vm::STORE4, OpcodePrinter("store4"))
    (vm::LSTORE4, OpcodePrinter("lstore4", 2))
    (vm::ADDI, OpcodePrinter("addi"))
    (vm::ADDF, OpcodePrinter("addf"))
    (vm::SUBI, OpcodePrinter("subi"))
    (vm::SUBF, OpcodePrinter("subf"))
    (vm::MULI, OpcodePrinter("muli"))
    (vm::MULF, OpcodePrinter("mulf"))
    (vm::DIVI, OpcodePrinter("divi"))
    (vm::DIVF, OpcodePrinter("divf"))
    (vm::MODI, OpcodePrinter("modi"))
    (vm::MODF, OpcodePrinter("modf"))
    (vm::NEGI, OpcodePrinter("negi"))
    (vm::NEGF, OpcodePrinter("negf"))
    (vm::ANDI, OpcodePrinter("andi"))
    (vm::ORI, OpcodePrinter("ori"))
    (vm::XORI, OpcodePrinter("xori"))
    (vm::NOTI, OpcodePrinter("noti"))

    (vm::I2F, OpcodePrinter("i2f"))
    (vm::F2I, OpcodePrinter("f2i"))

    (vm::JUMP, OpcodePrinter("jump"))
    (vm::CALL, OpcodePrinter("call"))
    (vm::ENTER, OpcodePrinter("enter", 2))
    (vm::LEAVE, OpcodePrinter("leave"))
    (vm::RET, OpcodePrinter("ret"))
    (vm::POP, OpcodePrinter("pop", 1))

    (vm::CJUMP, OpcodePrinter("cjump"))

    (vm::LTI, OpcodePrinter("lti"))
    (vm::LTF, OpcodePrinter("ltf"))
    (vm::LEI, OpcodePrinter("lei"))
    (vm::LEF, OpcodePrinter("lef"))
    (vm::GTI, OpcodePrinter("gti"))
    (vm::GTF, OpcodePrinter("gtf"))
    (vm::GEI, OpcodePrinter("gei"))
    (vm::GEF, OpcodePrinter("gef"))
    (vm::EQI, OpcodePrinter("eqi"))
    (vm::EQF, OpcodePrinter("eqf"))
    (vm::NEQI, OpcodePrinter("neqi"))
    (vm::NEQF, OpcodePrinter("neqf"))

    (vm::INPI, OpcodePrinter("inpi"))
    (vm::INPF, OpcodePrinter("inpf"))
    (vm::OUTI, OpcodePrinter("outi"))
    (vm::OUTF, OpcodePrinter("outf"));

}

typedef std::map<std::string, const ast::FunctionDef *> FunctionDefMap;


void generateFunction(const ast::FunctionDef& f, vm::BytecodeBuffer& bb, FunctionAddrMap& fam);


class VariableTable
{
public:

    VariableTable(BasicType returnType) : addrOfReturn_(8), returnType_(returnType) { }

    void insert(const ast::Variable *var, vm::BPAddr addr)
    {
        addr_.insert(std::make_pair(var, addr));
        
        vm::BPAddr aor = addr + ast::typeSize(var->type());
        if (aor > addrOfReturn_) addrOfReturn_ = aor;
    }

    bool erase(const ast::Variable *var)
    {
        return addr_.erase(var) > 0;
    }

    vm::BPAddr addrOf(const ast::Variable *var) const
    {
        auto it = addr_.find(var);

        assert(it != addr_.end());

        return it->second;
    }

    vm::BPAddr addrOfReturn() const { return addrOfReturn_; }
    BasicType returnType() const { return returnType_; }

private:
    typedef std::map<const ast::Variable *, vm::BPAddr> C;
    C addr_;
    vm::BPAddr addrOfReturn_;
    BasicType returnType_;
};

class NaiveStackAlloc
{
public:

    NaiveStackAlloc() : m_total(0) { }

    vm::BPAddr alloc(unsigned size)
    {
        m_total += size;
        return vm::BPAddr(-int(m_total));
    }

    void free(vm::BPAddr) { }

    unsigned peek() const { return m_total; }

private:

    unsigned m_total;    
};

class NormalStackAlloc
{
    typedef std::vector<std::uint8_t> C;

public:

    NormalStackAlloc() : peek_(0) { }

    vm::BPAddr alloc(unsigned size)
    {
        C::size_type i = 0;
        
        while (i < mem_.size() && mem_[i])
        {
            i += mem_[i];
        }

        if ((i + size) > mem_.size()) mem_.resize(i + size, 0);

        assert(size <= std::numeric_limits<C::value_type>::max());

        mem_[i] = C::value_type(size);

        if (peek_ < mem_.size()) peek_ = mem_.size();

        return -vm::BPAddr(i);
    }

    void free(vm::BPAddr addr)
    {
        addr = -addr;
        assert(addr >= 0 && addr < vm::BPAddr(mem_.size()) && mem_[addr] != 0);
        mem_[addr] = 0;
    }

    unsigned peek() const { return peek_; }

private:

    C mem_;
    unsigned peek_;
};

typedef NormalStackAlloc StackAlloc;




BasicType constantType(const ast::Constant& c);

void generateExpression(const ast::Expression& e, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt);
void generateExpressionRef(const ast::Expression& e, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt);

void gen_operator_plus_i(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 1);
    generateExpression(pc[0], cg, fam, salloc, vt);
}

void gen_operator_plus_f(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 1);
    generateExpression(pc[0], cg, fam, salloc, vt);
}

void gen_operator_minus_i(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 1);
    generateExpression(pc[0], cg, fam, salloc, vt);
    cg.emit(vm::NEGI);
}

void gen_operator_minus_f(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 1);
    generateExpression(pc[0], cg, fam, salloc, vt);
    cg.emit(vm::NEGF);
}

void gen_operator_lnot_b(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 1);
    generateExpression(pc[0], cg, fam, salloc, vt);
    cg.emit(vm::CONST4);
    cg.emit<std::int32_t>(1);
    cg.emit(vm::XORI);
}

#define SL_GEN_OPERATORS(name, opcode) \
void gen_operator_##name##_ii(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt) \
{ \
    assert(pc.size() == 2); \
    generateExpression(pc[0], cg, fam, salloc, vt); \
    generateExpression(pc[1], cg, fam, salloc, vt); \
    cg.emit(vm::opcode##I); \
} \
 \
void gen_operator_##name##_fi(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt) \
{ \
    assert(pc.size() == 2); \
    generateExpression(pc[0], cg, fam, salloc, vt); \
    generateExpression(pc[1], cg, fam, salloc, vt); \
    cg.emit(vm::I2F); \
    cg.emit(vm::opcode##F); \
} \
 \
void gen_operator_##name##_if(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt) \
{ \
    assert(pc.size() == 2); \
    generateExpression(pc[0], cg, fam, salloc, vt); \
    cg.emit(vm::I2F); \
    generateExpression(pc[1], cg, fam, salloc, vt); \
    cg.emit(vm::opcode##F); \
}  \
 \
void gen_operator_##name##_ff(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt) \
{ \
    assert(pc.size() == 2); \
    generateExpression(pc[0], cg, fam, salloc, vt); \
    generateExpression(pc[1], cg, fam, salloc, vt); \
    cg.emit(vm::opcode##F); \
}

SL_GEN_OPERATORS(plus, ADD)
SL_GEN_OPERATORS(minus, SUB)
SL_GEN_OPERATORS(mul, MUL)
SL_GEN_OPERATORS(div, DIV)
SL_GEN_OPERATORS(mod, MOD)
SL_GEN_OPERATORS(lt, LT)
SL_GEN_OPERATORS(le, LE)
SL_GEN_OPERATORS(gt, GT)
SL_GEN_OPERATORS(ge, GE)
SL_GEN_OPERATORS(eq, EQ)
SL_GEN_OPERATORS(neq, NEQ)

void gen_operator_land_bb(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 2);
    cg.emit(vm::CONST4);
    vm::CodeAddr c1 = cg.emit<std::int32_t>(0xdeadbeef);
    generateExpression(pc[0], cg, fam, salloc, vt);
    cg.emit(vm::CJUMP);
    cg.emit(vm::CONST4);
    cg.emit<std::int32_t>(0);
    cg.emit(vm::CONST4);
    vm::CodeAddr c2 = cg.emit<std::int32_t>(0xdeadbeef);
    cg.emit(vm::JUMP);
    cg.emit<std::int32_t>(c1, cg.code.size());
    generateExpression(pc[1], cg, fam, salloc, vt);
    cg.emit<std::int32_t>(c2, cg.code.size());
}

void gen_operator_lor_bb(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 2);
    cg.emit(vm::CONST4);
    vm::CodeAddr c1 = cg.emit<std::int32_t>(0xdeadbeef);
    generateExpression(pc[0], cg, fam, salloc, vt);
    cg.emit(vm::CONST4);
    cg.emit<std::int32_t>(1);
    cg.emit(vm::XORI);
    cg.emit(vm::CJUMP);
    cg.emit(vm::CONST4);
    cg.emit<std::int32_t>(1);
    cg.emit(vm::CONST4);
    vm::CodeAddr c2 = cg.emit<std::int32_t>(0xdeadbeef);
    cg.emit(vm::JUMP);
    cg.emit<std::int32_t>(c1, cg.code.size());
    generateExpression(pc[1], cg, fam, salloc, vt);
    cg.emit<std::int32_t>(c2, cg.code.size());
}

void gen_function_swap4(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 2);
    generateExpressionRef(pc[0], cg, fam, salloc, vt);
    generateExpression(pc[1], cg, fam, salloc, vt);
    generateExpressionRef(pc[1], cg, fam, salloc, vt);
    generateExpression(pc[0], cg, fam, salloc, vt);
    cg.emit(vm::STORE4);
    cg.emit(vm::STORE4);
}

void gen_function_geti(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& , StackAlloc& , VariableTable& )
{
    assert(pc.size() == 0);
    cg.emit(vm::INPI);
}

void gen_function_getf(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& , StackAlloc& , VariableTable& )
{
    assert(pc.size() == 0);
    cg.emit(vm::INPF);
}

void gen_function_put_i(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 1);
    generateExpression(pc[0], cg, fam, salloc, vt);
    cg.emit(vm::OUTI);
}

void gen_function_put_f(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    assert(pc.size() == 1);
    generateExpression(pc[0], cg, fam, salloc, vt);
    cg.emit(vm::OUTF);
}

typedef boost::function<void(const ast::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)> BuiltinFunctionGen;
typedef std::map<const ast::BuiltinFunction *, BuiltinFunctionGen> BuiltinFunctionGenerators;

BuiltinFunctionGenerators builtinFunctionGen = boost::assign::map_list_of
    (&builtin::operator_plus_i, &gen_operator_plus_i)
    (&builtin::operator_plus_f, &gen_operator_plus_f)
    (&builtin::operator_minus_i, &gen_operator_minus_i)
    (&builtin::operator_minus_f, &gen_operator_minus_f)
    (&builtin::operator_lnot_b, &gen_operator_lnot_b)
    (&builtin::operator_plus_ii, &gen_operator_plus_ii)
    (&builtin::operator_plus_fi, &gen_operator_plus_fi)
    (&builtin::operator_plus_if, &gen_operator_plus_if)
    (&builtin::operator_plus_ff, &gen_operator_plus_ff)
    (&builtin::operator_minus_ii, &gen_operator_minus_ii)
    (&builtin::operator_minus_fi, &gen_operator_minus_fi)
    (&builtin::operator_minus_if, &gen_operator_minus_if)
    (&builtin::operator_minus_ff, &gen_operator_minus_ff)
    (&builtin::operator_mul_ii, &gen_operator_mul_ii)
    (&builtin::operator_mul_fi, &gen_operator_mul_fi)
    (&builtin::operator_mul_if, &gen_operator_mul_if)
    (&builtin::operator_mul_ff, &gen_operator_mul_ff)
    (&builtin::operator_div_ii, &gen_operator_div_ii)
    (&builtin::operator_div_fi, &gen_operator_div_fi)
    (&builtin::operator_div_if, &gen_operator_div_if)
    (&builtin::operator_div_ff, &gen_operator_div_ff)
    (&builtin::operator_mod_ii, &gen_operator_mod_ii)
    (&builtin::operator_mod_fi, &gen_operator_mod_fi)
    (&builtin::operator_mod_if, &gen_operator_mod_if)
    (&builtin::operator_mod_ff, &gen_operator_mod_ff)

    (&builtin::operator_lt_ii, &gen_operator_lt_ii)
    (&builtin::operator_lt_fi, &gen_operator_lt_fi)
    (&builtin::operator_lt_if, &gen_operator_lt_if)
    (&builtin::operator_lt_ff, &gen_operator_lt_ff)

    (&builtin::operator_le_ii, &gen_operator_le_ii)
    (&builtin::operator_le_fi, &gen_operator_le_fi)
    (&builtin::operator_le_if, &gen_operator_le_if)
    (&builtin::operator_le_ff, &gen_operator_le_ff)

    (&builtin::operator_gt_ii, &gen_operator_gt_ii)
    (&builtin::operator_gt_fi, &gen_operator_gt_fi)
    (&builtin::operator_gt_if, &gen_operator_gt_if)
    (&builtin::operator_gt_ff, &gen_operator_gt_ff)

    (&builtin::operator_ge_ii, &gen_operator_ge_ii)
    (&builtin::operator_ge_fi, &gen_operator_ge_fi)
    (&builtin::operator_ge_if, &gen_operator_ge_if)
    (&builtin::operator_ge_ff, &gen_operator_ge_ff)

    (&builtin::operator_eq_ii, &gen_operator_eq_ii)
    (&builtin::operator_eq_fi, &gen_operator_eq_fi)
    (&builtin::operator_eq_if, &gen_operator_eq_if)
    (&builtin::operator_eq_ff, &gen_operator_eq_ff)
    (&builtin::operator_eq_bb, &gen_operator_eq_ii)

    (&builtin::operator_neq_ii, &gen_operator_neq_ii)
    (&builtin::operator_neq_fi, &gen_operator_neq_fi)
    (&builtin::operator_neq_if, &gen_operator_neq_if)
    (&builtin::operator_neq_ff, &gen_operator_neq_ff)
    (&builtin::operator_neq_bb, &gen_operator_neq_ii)
    
    (&builtin::operator_land_bb, &gen_operator_land_bb)
    (&builtin::operator_lor_bb, &gen_operator_lor_bb)

    (&builtin::function_swap_ii, &gen_function_swap4)
    (&builtin::function_swap_ff, &gen_function_swap4)
    (&builtin::function_swap_bb, &gen_function_swap4)
    
    (&builtin::function_geti, &gen_function_geti)
    (&builtin::function_getf, &gen_function_getf)
    (&builtin::function_put_i, &gen_function_put_i)
    (&builtin::function_put_f, &gen_function_put_f);

std::uint8_t parametersTotalSize(const ast::FunctionDef& fd)
{
    std::uint32_t size = 0;

    BOOST_FOREACH(const std::shared_ptr<ast::Variable>& p, fd.parameters())
    {
        size += ast::typeSize(p->type());
    }

    assert(size <= std::numeric_limits<std::uint8_t>::max());

    return std::uint8_t(size);
}

class GenerateFunctionCall : public boost::static_visitor<void>
{
public:

    GenerateFunctionCall(const ast::FunctionCall::ParamContainer& pc, bool discardReturnValue, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
        : pc_(pc), discardReturnValue_(discardReturnValue), cg_(cg), fam_(fam), salloc_(salloc), vt_(vt) { }

    void operator()(const ast::BuiltinFunction *bf) const
    {
        assert(builtinFunctionGen.find(bf) != builtinFunctionGen.end());

        builtinFunctionGen[bf](pc_, cg_, fam_, salloc_, vt_);

        if (discardReturnValue_ && bf->type() != void_)
        {
            cg_.emit(vm::POP);
            cg_.emit(std::uint8_t(ast::typeSize(bf->type())));
        }
    }

    void operator()(const ast::FunctionDef *fd) const
    {
        std::string fn = ast::functionMangledName(*fd);

        auto it = fam_.find(fn);

        assert(it != fam_.end());

        if (fd->type() != void_)
        {
            cg_.emit(vm::CONST4);
            assert(ast::typeSize(fd->type()) == sizeof(int_t));
            cg_.emit(std::int32_t(0)); // return value placeholder
        }

        assert(pc_.size() == fd->parameters().size());

        auto pd = fd->parameters().begin();
        auto e = pc_.begin();

        while (e != pc_.end())
        {
            if ((*pd)->ref())
            {
                generateExpressionRef(*e, cg_, fam_, salloc_, vt_);
            }
            else
            {
                generateExpression(*e, cg_, fam_, salloc_, vt_);
            }

            ++e;
            ++pd;
        }

        cg_.emit(vm::CONST4);
        cg_.emit<std::int32_t>(it->second);
        cg_.emit(vm::CALL);

        if (discardReturnValue_ && fd->type() != void_)
        {
            cg_.emit(vm::POP);
            cg_.emit(std::uint8_t(parametersTotalSize(*fd) + ast::typeSize(fd->type())));
        }
        else
        {
            if (parametersTotalSize(*fd) > 0)
            {
                cg_.emit(vm::POP);
                cg_.emit<std::uint8_t>(parametersTotalSize(*fd));
            }
        }
    }

private:

    const ast::FunctionCall::ParamContainer& pc_;
    bool discardReturnValue_;
    vm::CodeGenerator& cg_;
    FunctionAddrMap& fam_;
    StackAlloc& salloc_;
    VariableTable& vt_;
};

class GenerateConstant : public boost::static_visitor<void>
{
public:

    GenerateConstant(vm::CodeGenerator& cg) : cg_(cg) { }

    void operator()(float f) const
    {
        cg_.emit(vm::CONST4);
        cg_.emit(f);
    }

    void operator()(int i) const
    {
        cg_.emit(vm::CONST4);
        cg_.emit<std::int32_t>(i);
    }

    void operator()(bool b) const
    {
        cg_.emit(vm::CONST4);
        cg_.emit<std::int32_t>(b);
    }

private:
    vm::CodeGenerator& cg_;
};

class GenerateExpression : public boost::static_visitor<void>
{
public:

    GenerateExpression(vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
        : cg_(cg), fam_(fam), salloc_(salloc), vt_(vt) { }

    void operator()(const ast::Constant& c) const
    {
        GenerateConstant gc(cg_);
        c.value().apply_visitor(gc);
    }

    void operator()(const ast::Variable *v) const
    {
        if (v->ref())
        {
            cg_.emit(vm::LLOAD4);
            cg_.emit(std::int16_t(vt_.addrOf(v)));
            cg_.emit(vm::LOAD4);
        }
        else
        {
            cg_.emit(vm::LLOAD4);
            cg_.emit(std::int16_t(vt_.addrOf(v)));
        }
    }

    void operator()(const ast::FunctionCall& fc) const
    {
        GenerateFunctionCall gfc(fc.params(), false, cg_, fam_, salloc_, vt_);
        fc.f().apply_visitor(gfc);
    }

    void operator()(const ast::Cast& c) const
    {
        BasicType exprType = expressionType(c.expr());

        if (c.type() == exprType)
        {
            c.expr().apply_visitor(*this);
        }
        else
        {
            GenerateExpression ge(cg_, fam_, salloc_, vt_);
            c.expr().apply_visitor(ge);

            switch (c.type())
            {
                case int_: // float to int

                    cg_.emit(vm::F2I);
                    break;
                   
                case float_: // float to int

                    cg_.emit(vm::I2F);
                    break;
                   
                default:

                    assert(!"Bad type!");
            }
        }
    }

private:

    vm::CodeGenerator& cg_;
    FunctionAddrMap& fam_;
    StackAlloc& salloc_;
    VariableTable& vt_;
};

void generateExpression(const ast::Expression& e, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    GenerateExpression ge(cg, fam, salloc, vt);
    e.apply_visitor(ge);
}

class GenerateExpressionRef : public boost::static_visitor<void>
{
public:

    GenerateExpressionRef(vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
        : cg_(cg), fam_(fam), salloc_(salloc), vt_(vt) { }

    template <typename T>
    void operator()(const T& ) const
    {
        assert(!"Not an lvalue");
    }

    void operator()(const ast::Variable *v) const
    {
        cg_.emit(vm::LADDR);
        cg_.emit(std::int16_t(vt_.addrOf(v)));
    }

private:

    vm::CodeGenerator& cg_;
    FunctionAddrMap& fam_;
    StackAlloc& salloc_;
    VariableTable& vt_;
};

void generateExpressionRef(const ast::Expression& e, vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt)
{
    GenerateExpressionRef ge(cg, fam, salloc, vt);
    e.apply_visitor(ge);
}


typedef std::vector<vm::CodeAddr> ReturnAddresses;

class GenerateStatement : public boost::static_visitor<void>
{
public:

    GenerateStatement(vm::CodeGenerator& cg, FunctionAddrMap& fam, StackAlloc& salloc, VariableTable& vt, ReturnAddresses& ra)
        : cg_(cg), fam_(fam), salloc_(salloc), vt_(vt), ra_(ra) { }

    void operator()(const ast::CompoundStatement& cs) const
    {
        BOOST_FOREACH(const ast::Statement& s, cs.statements)
        {
            s.apply_visitor(*this);    
        }
    }

    void operator()(const ast::IfStatement& ifs) const
    {
        cg_.emit(vm::CONST4);
        vm::CodeAddr je = cg_.emit(std::int32_t(0xbebebebe));

        generateExpression(ifs.cond(), cg_, fam_, salloc_, vt_);

        cg_.emit(vm::CJUMP);

        if (ifs.onFalse()) (*this)(*ifs.onFalse());

        cg_.emit(vm::CONST4);
        vm::CodeAddr end = cg_.emit(std::int32_t(0xbebebebe));
        cg_.emit(vm::JUMP);

        cg_.emit(je, cg_.code.size());

        (*this)(ifs.onTrue());

        cg_.emit(end, cg_.code.size());
    }

    void operator()(const ast::WhileLoop& loop) const
    {
        vm::CodeAddr lloop = cg_.code.size();
        cg_.emit(vm::CONST4);
        vm::CodeAddr je = cg_.emit(std::int32_t(0xbebebebe));

        generateExpression(loop.cond(), cg_, fam_, salloc_, vt_);

        cg_.emit(vm::CJUMP);
        cg_.emit(vm::CONST4);
        vm::CodeAddr end = cg_.emit(std::int32_t(0xbebebebe));
        cg_.emit(vm::JUMP);
        cg_.emit(je, cg_.code.size());

        (*this)(loop.body());

        cg_.emit(vm::CONST4);
        cg_.emit(std::int32_t(lloop));
        cg_.emit(vm::JUMP);

        cg_.emit(end, cg_.code.size());
    }

    void operator()(const ast::Assignment& a) const
    {
        assert(a.var().type() == expressionType(a.expr()));

        if (a.var().ref())
        {
            cg_.emit(vm::LLOAD4);
            cg_.emit(std::int16_t(vt_.addrOf(&a.var())));
        }

        generateExpression(a.expr(), cg_, fam_, salloc_, vt_);

        if (a.var().ref())
        {
            cg_.emit(vm::STORE4);
        }
        else
        {
            cg_.emit(vm::LSTORE4);
            cg_.emit(std::int16_t(vt_.addrOf(&a.var())));
        }
    }

    void operator()(const ast::FunctionCall& fc) const
    {
        GenerateFunctionCall gfc(fc.params(), false, cg_, fam_, salloc_, vt_);
        fc.f().apply_visitor(gfc);
    }

    void operator()(const ast::ReturnStatement& rs) const
    {
        if (expressionType(rs.expr()) != vt_.returnType())
        {
            generateExpression(ast::Cast(expressionPos(rs.expr()), rs.expr(), vt_.returnType()), cg_, fam_, salloc_, vt_);
        }
        else
        {
            generateExpression(rs.expr(), cg_, fam_, salloc_, vt_);
        }

        cg_.emit(vm::LSTORE4);
        cg_.emit(std::int16_t(vt_.addrOfReturn()));
        cg_.emit(vm::CONST4);
        ra_.push_back(cg_.emit<std::int32_t>(0xbadabada));
        cg_.emit(vm::JUMP);
    }

    void operator()(const ast::VariableDecl& vd) const
    {
        vt_.insert(&vd.var(), salloc_.alloc(ast::typeSize(vd.var().type())) - ast::typeSize(vd.var().type()));

        if (vd.expr())
        {
            generateExpression(*vd.expr(), cg_, fam_, salloc_, vt_);

            cg_.emit(vm::LSTORE4);
            cg_.emit(std::int16_t(vt_.addrOf(&vd.var())));
        }
    }

    void operator()(const ast::VariableDelete& vd) const
    {
        // TODO: should be remove it from VT?
        //vt_.erase(&vd.var());
        salloc_.free(vt_.addrOf(&vd.var()) + ast::typeSize(vd.var().type()));
    }

private:

    vm::CodeGenerator& cg_;
    FunctionAddrMap& fam_;
    StackAlloc& salloc_;
    VariableTable& vt_;
    ReturnAddresses& ra_;
};

void allocParameters(const ast::FunctionDef& f, VariableTable& vt)
{
    int off = 4 + 4; // old BP, return address

    for (auto it = f.parameters().rbegin(); it != f.parameters().rend(); ++it)
    {
        vt.insert(&**it, vm::BPAddr(off));

        off += ast::typeSize((*it)->type());
    }
}

void generateFunction(const ast::FunctionDef& f, vm::CodeGenerator& cg, FunctionAddrMap& fam)
{
    vm::CodeAddr addr = cg.emit(vm::ENTER);
    cg.emit<std::uint16_t>(0);

    fam.insert(std::make_pair(functionMangledName(f), addr));

    StackAlloc salloc;
    VariableTable vt(f.type());

    allocParameters(f, vt);

    ReturnAddresses ra;

    GenerateStatement(cg, fam, salloc, vt, ra)(*f.body());

    std::int32_t peek = salloc.peek();

    cg.emit(addr + 1, std::uint16_t(peek));

    vm::CodeAddr retAddr = cg.emit(vm::LEAVE);
    cg.emit(vm::RET);

    BOOST_FOREACH(vm::CodeAddr a, ra)
    {
        cg.emit(a, std::int32_t(retAddr));
    }
}

class FunctionCallDependencies : public boost::static_visitor<void>
{
public:

    FunctionCallDependencies(DependencyGraph<ast::FunctionDef>& dg, const ast::FunctionDef *fd)
        : dg_(dg), fd_(fd) { }

    void operator()(const ast::BuiltinFunction *) const
    {
    }

    void operator()(const ast::FunctionDef *fd) const
    {
        dg_.insert(fd_, fd);
    }

private:

    DependencyGraph<ast::FunctionDef>& dg_;
    const ast::FunctionDef *fd_;
};

class ExpressionDependencies : public boost::static_visitor<void>
{
public:

    ExpressionDependencies(DependencyGraph<ast::FunctionDef>& dg, const ast::FunctionDef *fd)
        : dg_(dg), fd_(fd) { }

    void operator()(const ast::Constant& ) const { }

    void operator()(const ast::Variable *) const { }

    void operator()(const ast::FunctionCall& fc) const
    {
        BOOST_FOREACH(const ast::Expression& e, fc.params())
        {
            e.apply_visitor(*this);    
        }

        FunctionCallDependencies fcd(dg_, fd_);
        fc.f().apply_visitor(fcd);
    }

    void operator()(const ast::Cast& c) const
    {
        c.expr().apply_visitor(*this);
    }

private:

    DependencyGraph<ast::FunctionDef>& dg_;
    const ast::FunctionDef *fd_;
};

class StatementDependencies : public boost::static_visitor<void>
{
public:

    StatementDependencies(DependencyGraph<ast::FunctionDef>& dg, const ast::FunctionDef *fd)
        : dg_(dg), fd_(fd) { }

    void operator()(const ast::CompoundStatement& cs) const
    {
        BOOST_FOREACH(const ast::Statement& s, cs.statements)
        {
            s.apply_visitor(*this);    
        }
    }

    void operator()(const ast::IfStatement& ifs) const
    {
        ExpressionDependencies ed(dg_, fd_);
        ifs.cond().apply_visitor(ed);
        (*this)(ifs.onTrue());

        if (ifs.onFalse()) (*this)(*ifs.onFalse());
    }

    void operator()(const ast::WhileLoop& loop) const
    {
        ExpressionDependencies ed(dg_, fd_);
        loop.cond().apply_visitor(ed);
        (*this)(loop.body());
    }

    void operator()(const ast::Assignment& a) const
    {
        ExpressionDependencies ed(dg_, fd_);
        a.expr().apply_visitor(ed);
    }

    void operator()(const ast::FunctionCall& fc) const
    {
        ExpressionDependencies(dg_, fd_)(fc);
    }

    void operator()(const ast::ReturnStatement& rs) const
    {
        ExpressionDependencies ed(dg_, fd_);
        rs.expr().apply_visitor(ed);
    }

    void operator()(const ast::VariableDecl& vd) const
    {
        if (vd.expr())
        {
            ExpressionDependencies ed(dg_, fd_);
            vd.expr()->apply_visitor(ed);
        }
    }

    void operator()(const ast::VariableDelete& ) const { }

private:

    DependencyGraph<ast::FunctionDef>& dg_;
    const ast::FunctionDef *fd_;
};

void generateDependencies(const ast::FunctionDef& f, DependencyGraph<ast::FunctionDef>& dg)
{
    BOOST_FOREACH(const ast::Statement& s, f.body()->statements)
    {
        StatementDependencies sd(dg, &f);
        s.apply_visitor(sd);
    }
}

vm::BytecodeBuffer generateBytecode(const ast::Module& module, FunctionAddrMap& fam)
{
    vm::CodeGenerator cg;
    FunctionDefMap fdm;

    BOOST_FOREACH(const ast::FunctionDef& f, module.functions())
    {
        assert(f.body());

        fdm[ast::functionMangledName(f.name(), f.suffix())] = &f;
    }

    cg.emit(vm::CONST4);
    cg.emit<std::int32_t>(0);
    cg.emit(vm::CONST4);
    cg.emit<std::int32_t>(-1);
    cg.emit(vm::CONST4);
    vm::CodeAddr startAddr = cg.emit<std::int32_t>(0);
    cg.emit(vm::JUMP);

    DependencyGraph<ast::FunctionDef> funcDep;

    BOOST_FOREACH(const ast::FunctionDef& f, module.functions())
    {
        generateDependencies(f, funcDep);
    }

    BOOST_FOREACH(const ast::FunctionDef& f, module.functions())
    {
        funcDep.visit(&f, [&cg, &fam](const ast::FunctionDef *f)
            {
                std::string fmn = ast::functionMangledName(f);

                if (fam.find(fmn) == fam.end()) generateFunction(*f, cg, fam);
            });
    }

    vm::CodeAddr faddr = fam.find("main$")->second;

    cg.emit<std::int32_t>(startAddr, faddr);

    return cg.code;
}

void exportToAsm(const vm::BytecodeBuffer& bb, std::ostream& os)
{
    // find labels

    std::vector<std::string> labels;

    labels.resize(bb.size());

    for (vm::CodeAddr i = 0; i != bb.size(); i += opcodePrinters[bb[i]].size())
    {
        if (bb[i] == vm::CONST4 && (i + 6) <= bb.size())
        {
            if (bb[i + 5] == vm::CALL || bb[i + 5] == vm::JUMP)
            {
                std::uint32_t off;
                std::memcpy(&off, &bb[i + 1], sizeof(off));
                labels[off] = (bb[i + 5] == vm::CALL) ? "f" : "l";
            }
        }
    }

    {
        unsigned fn = 1;
        unsigned ln = 1;
    
        for (auto it = labels.begin(); it != labels.end(); ++it)
        {
            std::ostringstream os;
            os << *it;
            if (*it == "f")
            {
                os << fn++;
            }
            else if (*it == "l")
            {
                os << ln++;
            }

            *it = os.str();
        }
    }

    for (auto it = bb.begin(); it != bb.end(); )
    {
        std::ptrdiff_t i = it - bb.begin();
        if (!labels[i].empty())
        {
            os << labels[i] << ":" << std::endl;
        }

        os << "\t";

        it = opcodePrinters[*it](it, os, labels);
        
        os << std::endl;
    }


}

}
