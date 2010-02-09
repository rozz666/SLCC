#include <boost/assign/list_of.hpp>
#include "codeGen.hpp"

namespace sl
{

st::Type expressionType(const st::Expression& expr);

namespace
{

class OpcodePrinter
{
public:

    OpcodePrinter() : name_("XXX") { }

    OpcodePrinter(const std::string& name) : name_(name), argSize_(0) { }
    OpcodePrinter(const std::string& name, unsigned argSize) : name_(name), argSize_(argSize) { }

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

            if (val >= 0 && val < std::int32_t(labels.size()) && !labels[val].empty()) os << labels[val];
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
};

std::map<std::uint8_t, OpcodePrinter> opcodePrinters = boost::assign::map_list_of
    (vm::NOP, OpcodePrinter("nop"))
    (vm::CONST4, OpcodePrinter("const4", 4))
    (vm::LOAD4, OpcodePrinter("load4", 2))
    (vm::STORE4, OpcodePrinter("store4", 2))
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
    (vm::I2F, OpcodePrinter("i2f"))
    (vm::F2I, OpcodePrinter("f2i"))

    (vm::JUMP, OpcodePrinter("jump"))
    (vm::CALL, OpcodePrinter("call"))
    (vm::ENTER, OpcodePrinter("enter", 2))
    (vm::LEAVE, OpcodePrinter("leave"))
    (vm::RET, OpcodePrinter("ret"))
    (vm::POP, OpcodePrinter("pop", 1))

    (vm::INPI, OpcodePrinter("inpi", 2))
    (vm::OUTI, OpcodePrinter("outi", 2));

}

typedef std::map<std::string, const st::FunctionDef *> FunctionDefMap;
typedef std::map<std::string, vm::CodeAddr> FunctionAddrMap;


void generateFunction(const st::FunctionDef& f, vm::BytecodeBuffer& bb, FunctionAddrMap& fam);

unsigned typeSize(st::Type type)
{
    switch (type)
    {
        case st::int_: return 4;
        case st::float_: return 4;
    }

    assert(!"Bad type!");

    return 0;
}

class VariableTable
{
public:

    void insert(const st::Variable *var, vm::BPAddr addr)
    {
        addr_.insert(std::make_pair(var, addr));
    }

    vm::BPAddr addrOf(const st::Variable *var)
    {
        auto it = addr_.find(var);

        assert(it != addr_.end());

        return it->second;
    }

private:
    typedef std::map<const st::Variable *, vm::BPAddr> C;
    C addr_;
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

st::Type constantType(const st::Constant& c);

void generateExpression(const st::Expression& e, vm::CodeGenerator& cg, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt);

class GenerateFunctionCall : public boost::static_visitor<void>
{
public:

    GenerateFunctionCall(const st::FunctionCall::ParamContainer& pc, vm::CodeGenerator& cg, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt)
        : pc_(pc), cg_(cg), fam_(fam), salloc_(salloc), vt_(vt) { }

    void operator()(const st::BuiltinFunction *bf) const
    {
        if (bf->name() == "operator+")
        {
            //BPAddr tmp1 = 
        }
    }

    void operator()(const st::FunctionDef *fd) const
    {
        std::string fn = st::functionMangledName(*fd);

        auto it = fam_.find(fn);
/*        
        if (it == fam_->end())
        {
            generateFunction(*fd, *bb_, *fam_); 

            it = fam_->find(fn);
        }*/

        assert(it != fam_.end());

        cg_.emit(vm::CONST4);
        cg_.emit(std::int32_t(0)); // return value placeholder

        BOOST_FOREACH(const st::Expression& e, pc_)
        {
            generateExpression(e, cg_, fam_, salloc_, vt_);
        }

        cg_.emit(vm::CONST4);
        cg_.emit<std::int32_t>(it->second);
        cg_.emit(vm::CALL);
    }

private:

    const st::FunctionCall::ParamContainer& pc_;
    vm::CodeGenerator& cg_;
    FunctionAddrMap& fam_;
    NaiveStackAlloc& salloc_;
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

private:
    vm::CodeGenerator& cg_;
};

class GenerateExpression : public boost::static_visitor<void>
{
public:

    GenerateExpression(vm::CodeGenerator& cg, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt)
        : cg_(cg), fam_(fam), salloc_(salloc), vt_(vt) { }

    void operator()(const st::Constant& c) const
    {
        GenerateConstant gc(cg_);
        c.apply_visitor(gc);
    }

    void operator()(const st::Variable *v) const
    {
        cg_.emit(vm::LOAD4);
        cg_.emit(std::int16_t(vt_.addrOf(v)));
    }

    void operator()(const st::FunctionCall& fc) const
    {
        GenerateFunctionCall gfc(fc.params(), cg_, fam_, salloc_, vt_);
        fc.f().apply_visitor(gfc);
    }

    void operator()(const st::Cast& c) const
    {
        st::Type exprType = expressionType(c.expr());

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
                case st::int_: // float to int

                    cg_.emit(vm::F2I);
                    break;
                   
                case st::float_: // float to int

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
    NaiveStackAlloc& salloc_;
    VariableTable& vt_;
};

void generateExpression(const st::Expression& e, vm::CodeGenerator& cg, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt)
{
    GenerateExpression ge(cg, fam, salloc, vt);
    e.apply_visitor(ge);
}

class GenerateStatement : public boost::static_visitor<void>
{
public:

    GenerateStatement(vm::CodeGenerator& cg, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt)
        : cg_(cg), fam_(fam), salloc_(salloc), vt_(vt) { }

    void operator()(const st::CompoundStatement& cs) const
    {
        BOOST_FOREACH(const st::Statement& s, cs.statements)
        {
            s.apply_visitor(*this);    
        }
    }

    void operator()(const st::Assignment& a) const
    {
        assert(a.var().type() == expressionType(a.expr()));

        generateExpression(a.expr(), cg_, fam_, salloc_, vt_);
        cg_.emit(vm::STORE4);
        cg_.emit(std::int16_t(vt_.addrOf(&a.var())));
    }

    void operator()(const st::FunctionCall& ) const
    {
        assert(!"No function calls supported yet!");
    }

    void operator()(const st::ReturnStatement& rs) const
    {
        GenerateExpression ge(cg_, fam_, salloc_, vt_);
        rs.expr().apply_visitor(ge); // TODO: add cast
        cg_.emit(vm::STORE4);
        cg_.emit(std::int16_t(0)); // TODO: return value address
    }

    void operator()(const st::VariableDecl& vd) const
    {
        vt_.insert(&vd.var(), salloc_.alloc(typeSize(vd.var().type())));
    }

private:

    vm::CodeGenerator& cg_;
    FunctionAddrMap& fam_;
    NaiveStackAlloc& salloc_;
    VariableTable& vt_;
};

void allocParameters(const st::FunctionDef& f, VariableTable& vt)
{
    int off = 12;

    BOOST_FOREACH(const std::shared_ptr<st::Variable>& p, f.parameters())
    {
        vt.insert(&*p, vm::BPAddr(off));

        off += typeSize(p->type());
    }
}

void generateFunction(const st::FunctionDef& f, vm::CodeGenerator& cg, FunctionAddrMap& fam)
{
    vm::CodeAddr addr = cg.emit(vm::ENTER);
    cg.emit<std::uint16_t>(0);

    fam.insert(std::make_pair(functionMangledName(f), addr));

    NaiveStackAlloc salloc;
    VariableTable vt;

    allocParameters(f, vt);


    GenerateStatement(cg, fam, salloc, vt)(*f.body());

    std::int32_t peek = salloc.peek();

    cg.emit(addr + 1, std::uint16_t(peek));

    cg.emit(vm::LEAVE);
    cg.emit(vm::RET);
}

vm::BytecodeBuffer generateBytecode(const st::Module& module)
{
    vm::CodeGenerator cg;
    FunctionDefMap fdm;
    FunctionAddrMap fam;

    BOOST_FOREACH(const st::FunctionDef& f, module.functions())
    {
        assert(f.body());

        fdm[st::functionMangledName(f.name(), f.suffix())] = &f;
    }

    cg.emit(vm::CONST4);
    cg.emit<std::int32_t>(0);
    cg.emit(vm::CONST4);
    cg.emit<std::int32_t>(-1);
    cg.emit(vm::CONST4);
    vm::CodeAddr startAddr = cg.emit<std::int32_t>(0);
    cg.emit(vm::JUMP);

    BOOST_FOREACH(const st::FunctionDef& f, module.functions())
    {
        std::string fmn = st::functionMangledName(f);

        if (fam.find(fmn) == fam.end()) generateFunction(f, cg, fam);
    }

    vm::CodeAddr faddr = fam.find("main$")->second;

    cg.emit<std::int32_t>(startAddr, faddr);

    return cg.code();
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
                labels[off] = "f";
            }
        }
    }

    {
        unsigned n = 1;
    
        for (auto it = labels.begin(); it != labels.end(); ++it)
        {
            if (!it->empty())
            {
                std::ostringstream os;
                os << *it << n++;
                *it = os.str();
            }
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
