#include <boost/assign/list_of.hpp>
#include "codeGen.hpp"

namespace sl
{

st::Type expressionType(const st::Expression& expr);

namespace
{

std::map<opcodes::Tag, std::string> tagNames = boost::assign::map_list_of
    (opcodes::MOV, "mov")
    (opcodes::ADD, "add")
    (opcodes::SUB, "sub")
    (opcodes::MUL, "mul")
    (opcodes::DIV, "div")
    (opcodes::MOD, "mod")
    
    (opcodes::CALL, "call") 
    (opcodes::ENTER, "enter")
    (opcodes::LEAVE, "leave")
    (opcodes::RET, "return")
    
    (opcodes::PUSH, "push")
    (opcodes::INCSP, "incsp")
    
    (opcodes::EXIT, "exit")

    (opcodes::INTTOREAL, "inttoreal")
    (opcodes::REALTOINT, "realtoint");

}

typedef std::map<std::string, const st::FunctionDef *> FunctionDefMap;
typedef std::map<std::string, CodeAddr> FunctionAddrMap;


void generateFunction(const st::FunctionDef& f, BytecodeBuffer& bb, FunctionAddrMap& fam);

opcodes::Type convertType(st::Type type)
{
    switch (type)
    {
        case st::int_: return opcodes::INT;
        case st::float_: return opcodes::REAL;
    }

    assert(!"Bad type!");

    return opcodes::INT;
}

unsigned typeSize(opcodes::Type type)
{
    switch (type)
    {
        case opcodes::INT: return 4;
        case opcodes::REAL: return 8;
    }

    assert(!"Bad type!");

    return 0;
}

std::string typeSuffix(opcodes::Type type)
{
    switch (type)
    {
        case opcodes::INT: return ".i";
        case opcodes::REAL: return ".r";
    }

    assert(!"Bad type!");

    return "";
}

class VariableTable
{
public:

    void insert(const st::Variable *var, BPAddr addr)
    {
        addr_.insert(std::make_pair(var, addr));
    }

    BPAddr addrOf(const st::Variable *var)
    {
        auto it = addr_.find(var);

        assert(it != addr_.end());

        return it->second;
    }

private:
    typedef std::map<const st::Variable *, BPAddr> C;
    C addr_;
};

class NaiveStackAlloc
{
public:

    NaiveStackAlloc() : m_total(0) { }

    BPAddr alloc(unsigned size)
    {
        m_total += size;
        return BPAddr(-int(m_total));
    }

    void free(BPAddr) { }

    unsigned peek() const { return m_total; }

private:

    unsigned m_total;    
};

struct ConstantType : public boost::static_visitor<opcodes::Type>
{
    opcodes::Type operator()(int) const { return opcodes::INT; }
    opcodes::Type operator()(float) const { return opcodes::REAL; }
};

opcodes::Type constantType(const st::Constant& c)
{
    ConstantType ct;
    return c.apply_visitor(ct);
}

template <typename DestAddr>
void generateExpression(const st::Expression& e, DestAddr destAddr, BytecodeBuffer& bb, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt);


BP bpRef(BPAddr addr)
{
    return BP(addr.off);
}

BPAddr bpRef(BPDeref deref)
{
    return BPAddr(deref.off);
}

template <typename DestAddr>
class GenerateFunctionCall : public boost::static_visitor<void>
{
public:

    GenerateFunctionCall(DestAddr destAddr, const st::FunctionCall::ParamContainer& pc, BytecodeBuffer& bb, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt)
        : destAddr_(destAddr), pc_(&pc), bb_(&bb), fam_(&fam), salloc_(&salloc), vt_(&vt) { }

    void operator()(const st::BuiltinFunction *bf) const
    {
        namespace oc = opcodes;

        if (bf->name() == "operator+")
        {
            //BPAddr tmp1 = 
        }
    }

    void operator()(const st::FunctionDef *fd) const
    {
        namespace oc = opcodes;

        std::string fn = st::functionMangledName(*fd);

        auto it = fam_->find(fn);
/*        
        if (it == fam_->end())
        {
            generateFunction(*fd, *bb_, *fam_); 

            it = fam_->find(fn);
        }*/

        assert(it != fam_->end());
        unsigned paramTotal = 4; // ret

        BOOST_FOREACH(const st::Expression& e, *pc_)
        {
            oc::Type paramType = convertType(expressionType(e));
            unsigned paramSize = typeSize(paramType);
            BPAddr tmp = salloc_->alloc(paramSize);
            paramTotal += paramSize;

            generateExpression(e, tmp, *bb_, *fam_, *salloc_, *vt_);

            bb_->append(oc::Opcode(oc::PUSH, paramType, tmp));

            salloc_->free(tmp);
        }

        bb_->append(oc::Opcode(oc::PUSH, oc::INT, bpRef(destAddr_)));
        bb_->append(oc::Opcode(oc::CALL, it->second));
        bb_->append(oc::Opcode(oc::INCSP, oc::INT, int(paramTotal)));
    }

private:

    DestAddr destAddr_;
    const st::FunctionCall::ParamContainer *pc_;
    BytecodeBuffer *bb_;
    FunctionAddrMap *fam_;
    NaiveStackAlloc *salloc_;
    VariableTable *vt_;
};

template <typename DestAddr>
class GenerateExpression : public boost::static_visitor<void>
{
public:

    GenerateExpression(DestAddr destAddr, BytecodeBuffer& bb, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt)
        : destAddr_(destAddr), bb_(&bb), fam_(&fam), salloc_(&salloc), vt_(&vt) { }

    void operator()(const st::Constant& c) const
    {
        namespace oc = opcodes;

        bb_->append(oc::Opcode(oc::MOV, constantType(c), c, destAddr_));
    }

    void operator()(const st::Variable *v) const
    {
        namespace oc = opcodes;

        bb_->append(oc::Opcode(oc::MOV, convertType(v->type()), vt_->addrOf(v), destAddr_));
    }

    void operator()(const st::FunctionCall& fc) const
    {
        GenerateFunctionCall<DestAddr> gfc(destAddr_, fc.params(), *bb_, *fam_, *salloc_, *vt_);
        fc.f().apply_visitor(gfc);
    }

    void operator()(const st::Cast& c) const
    {
        namespace oc = opcodes;

        st::Type exprType = expressionType(c.expr());

        if (c.type() == exprType)
        {
            c.expr().apply_visitor(*this);
        }
        else
        {
            BPAddr tmp = salloc_->alloc(typeSize(convertType(exprType)));

            GenerateExpression<BPAddr> ge(tmp, *bb_, *fam_, *salloc_, *vt_);
            c.expr().apply_visitor(ge);

            switch (c.type())
            {
                case st::int_: // float to int

                    bb_->append(oc::Opcode(oc::REALTOINT, tmp, destAddr_));
                    break;
                   
                case st::float_: // float to int

                    bb_->append(oc::Opcode(oc::INTTOREAL, tmp, destAddr_));
                    break;
                   
                default:

                    assert(!"Bad type!");
            }

            salloc_->free(tmp);
        }
    }

private:

    DestAddr destAddr_;
    BytecodeBuffer *bb_;
    FunctionAddrMap *fam_;
    NaiveStackAlloc *salloc_;
    VariableTable *vt_;
};

template <typename DestAddr>
void generateExpression(const st::Expression& e, DestAddr destAddr, BytecodeBuffer& bb, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt)
{
    GenerateExpression<DestAddr> ge(destAddr, bb, fam, salloc, vt);
    e.apply_visitor(ge);
}

class GenerateStatement : public boost::static_visitor<void>
{
public:

    GenerateStatement(BytecodeBuffer& bb, FunctionAddrMap& fam, NaiveStackAlloc& salloc, VariableTable& vt)
        : bb_(&bb), fam_(&fam), salloc_(&salloc), vt_(&vt) { }

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

        GenerateExpression<BPAddr> ge(vt_->addrOf(&a.var()), *bb_, *fam_, *salloc_, *vt_);
        a.expr().apply_visitor(ge);
    }

    void operator()(const st::FunctionCall& ) const
    {
        assert(!"No function calls supported yet!");
    }

    void operator()(const st::ReturnStatement& rs) const
    {
        GenerateExpression<BPDeref> ge(BPDeref(8), *bb_, *fam_, *salloc_, *vt_);
        rs.expr().apply_visitor(ge); // TODO: add cast
    }

    void operator()(const st::VariableDecl& vd) const
    {
        vt_->insert(&vd.var(), salloc_->alloc(typeSize(convertType(vd.var().type()))));
    }

private:

    BytecodeBuffer *bb_;
    FunctionAddrMap *fam_;
    NaiveStackAlloc *salloc_;
    VariableTable *vt_;
};

void allocParameters(const st::FunctionDef& f, VariableTable& vt)
{
    int off = 12;

    BOOST_FOREACH(const std::shared_ptr<st::Variable>& p, f.parameters())
    {
        vt.insert(&*p, BPAddr(off));

        off += typeSize(convertType(p->type()));
    }
}

void generateFunction(const st::FunctionDef& f, BytecodeBuffer& bb, FunctionAddrMap& fam)
{
    namespace oc = opcodes;

    CodeAddr addr = bb.append(oc::Opcode(oc::ENTER, 0));

    fam.insert(std::make_pair(functionMangledName(f), addr));

    NaiveStackAlloc salloc;
    VariableTable vt;

    allocParameters(f, vt);


    GenerateStatement(bb, fam, salloc, vt)(*f.body());

    bb[addr] = oc::Opcode(oc::ENTER, int(salloc.peek()));

    bb.append(oc::Opcode(oc::LEAVE));
    bb.append(oc::Opcode(oc::RET));
}

BytecodeBuffer generateBytecode(const st::Module& module)
{
    namespace oc = opcodes;

    BytecodeBuffer bb;
    FunctionDefMap fdm;
    FunctionAddrMap fam;

    BOOST_FOREACH(const st::FunctionDef& f, module.functions())
    {
        assert(f.body());

        fdm[st::functionMangledName(f.name(), f.suffix())] = &f;
    }

    bb.append(oc::Opcode(oc::PUSH, 0));
    CodeAddr startAddr = bb.append(oc::Opcode(oc::CALL, 0));
    bb.append(oc::Opcode(oc::EXIT));

    BOOST_FOREACH(const st::FunctionDef& f, module.functions())
    {
        std::string fmn = st::functionMangledName(f);

        if (fam.find(fmn) == fam.end()) generateFunction(f, bb, fam);
    }

    bb[startAddr] = oc::Opcode(oc::CALL, fam.find("main$")->second);

    return bb;
}

class PrintArg : public boost::static_visitor<void>
{
public:

    PrintArg(std::ostream& os, const std::vector<std::string>& labels) : os_(&os), labels_(&labels) { }

    void operator()(const DataAddr& da) const
    {
        *os_ << da.off;
    }

    void operator()(const CodeAddr& ca) const
    {
        assert(!(*labels_)[ca.off].empty());

        *os_ << (*labels_)[ca.off];
    }

    void operator()(const BP& bp) const
    {
        *os_ << '#';

        (*this)(BPAddr(bp.off));
    }

    void operator()(const BPAddr& bp) const
    {
        *os_ << "BP";
        
        if (bp.off > 0) *os_ << '+';
        
        *os_ << bp.off;
    }

    void operator()(const BPDeref& bp) const
    {
        *os_ << '*';

        (*this)(BPAddr(bp.off));
    }

    void operator()(int i) const
    {
        *os_ << '#' << i;
    }

    void operator()(float f) const
    {
        *os_ << '#' << f;
    }

private:

    std::ostream *os_;
    const std::vector<std::string> *labels_;
};

void printOpcode(const opcodes::Opcode& oc, std::ostream& os, const std::vector<std::string>& labels)
{
    PrintArg pa(os, labels);

    os << tagNames[oc.tag()];

    if (oc.type()) os << typeSuffix(*oc.type());

    os << "\t";

    for (int i = 0; i < 3 && oc.arg(i); ++i)
    {
        if (i != 0) os << ", ";
        oc.arg(i)->apply_visitor(pa);
    }
}

void exportToAsm(const BytecodeBuffer& bb, std::ostream& os)
{
    namespace oc = opcodes;

    // find labels

    std::vector<std::string> labels;

    labels.resize(bb.size());

    for (unsigned i = 0; i != bb.size(); ++i)
    {
        if (bb[CodeAddr(i)].tag() == oc::CALL) labels[boost::get<CodeAddr>(*bb[CodeAddr(i)].arg(0)).off] = "f";
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

    for (unsigned i = 0; i != bb.size(); ++i)
    {
        if (!labels[i].empty())
        {
            os << labels[i] << ":" << std::endl;
        }

        os << "\t";

        printOpcode(bb[CodeAddr(i)], os, labels);
        
        os << std::endl;
    }


}

}
