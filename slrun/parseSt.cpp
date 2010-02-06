#include <iostream>
#include <boost/foreach.hpp>
#include "parseSt.hpp"

namespace sl
{


st::Type convertType(ast::Type type)
{
    switch (type)
    {
        case ast::int_: return st::int_;
        case ast::float_: return st::float_;
    }

    assert(!"Invalid type");

    return st::Type();
}

const char *operatorName(ast::Sign s)
{
    switch (s)
    {
        case ast::plus_: return "operator+";
        case ast::minus_: return "operator-";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(ast::MulOp o)
{
    switch (o)
    {
        case ast::mul_: return "operator*";
        case ast::div_: return "operator/";
        case ast::mod_: return "operator%";
    }

    assert(!"Not all operator handled");

    return "";
}

struct ConstantType : public boost::static_visitor<st::Type>
{
    st::Type operator()(const int& ) const
    {
        return st::int_;
    }

    st::Type operator()(const float& ) const
    {
        return st::float_;
    }
};

st::Type constantType(const st::Constant& c)
{
    ConstantType ct;
    return c.apply_visitor(ct);
}

struct ExpressionType : public boost::static_visitor<st::Type>
{
    st::Type operator()(const st::Constant& c) const
    {
        return constantType(c);
    }

    st::Type operator()(const st::Variable *v) const
    {
        return v->type();
    }

    st::Type operator()(const st::FunctionCall& fc) const
    {
        return functionType(fc.f());
    }

    st::Type operator()(const st::Cast& c) const
    {
        return c.type();
    }
};

st::Type expressionType(const st::Expression& expr)
{
    ExpressionType et;
    return expr.apply_visitor(et);
}


boost::optional<st::Expression> parseExpression(const ast::Expression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft);


boost::optional<st::FunctionCall> makeUnaryOperatorCall(const std::string& opName, const st::Expression& right, const st::FunctionTable& ft)
{
    st::Type rightType = expressionType(right);

    if (boost::optional<st::FunctionRef> f = ft.find(st::functionMangledName(opName, typeSuffix(rightType))))
    {
        return st::FunctionCall(*f, &right, &right + 1);
    }
    else
    {
        std::cerr << "operator not found: " << opName << "(" << typeName(rightType) << ")" << std::endl;

        return boost::none;
    }
}

class ParseFactor : public boost::static_visitor<boost::optional<st::Expression> >
{
public:

    ParseFactor(st::VariableTableStack& vts, const st::FunctionTable& ft) : vts_(&vts), ft_(&ft) { }

    boost::optional<st::Expression> operator()(const ast::Constant& c) const
    {
        return c;
    }

    boost::optional<st::Expression> operator()(const ast::Variable& v) const
    {
        const st::Variable *pv = vts_->find(v.name);

        if (!pv)
        {
            std::cerr << "unknown identifier: " << v.name << std::endl;
        }

        return pv;
    }

    boost::optional<st::Expression> operator()(const ast::FunctionCall& fc) const
    {
        std::vector<st::Expression> expr;
        std::string suffix;

        BOOST_FOREACH(const ast::Expression& e, fc.expr)
        {
            if (boost::optional<st::Expression> pe = parseExpression(e, *vts_, *ft_))
            {
                suffix += st::typeSuffix(expressionType(*pe));

                expr.push_back(*pe);
            }
        }

        if (expr.size() < fc.expr.size()) return boost::none;

        boost::optional<st::FunctionRef> f = ft_->find(st::functionMangledName(fc.name, suffix));

        if (!f)
        {
            std::cerr << "no function " << fc.name << " matching given parameters" << std::endl;

            return boost::none;
        }

        st::FunctionCall out(*f, expr.begin(), expr.end());

        return out;
    }

    boost::optional<st::Expression> operator()(const ast::Expression& e) const
    {
        return parseExpression(e, *vts_, *ft_);
    }

    boost::optional<st::Expression> operator()(const ast::SignedFactor& sf) const
    {
        switch (sf.sign)
        {
            case ast::plus_: return sf.factor.apply_visitor(*this);

            default:

                if (boost::optional<st::Expression> e = sf.factor.apply_visitor(*this))
                {
                    if (boost::optional<st::FunctionCall> fc = makeUnaryOperatorCall(operatorName(sf.sign), *e, *ft_))
                    {
                        return *fc;
                    }
                }
                
                return boost::none;
        }
    }

private:

    st::VariableTableStack *vts_;
    const st::FunctionTable *ft_;
};

boost::optional<st::Expression> parseFactor(const ast::Factor& f, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    ParseFactor pf(vts, ft);
    return f.apply_visitor(pf);
}

boost::optional<st::FunctionCall> makeBinaryOperatorCall(const std::string& opName, const st::Expression& left, const st::Expression& right, const st::FunctionTable& ft)
{
    st::Type leftType = expressionType(left);
    st::Type rightType = expressionType(right);

    if (boost::optional<st::FunctionRef> f = ft.find(st::functionMangledName(opName, std::string(typeSuffix(leftType)) + typeSuffix(rightType))))
    {
        std::vector<st::Expression> expr;

        expr.reserve(2);
        expr.push_back(left);
        expr.push_back(right);

        return st::FunctionCall(*f, expr.begin(), expr.end());
    }
    else
    {
        std::cerr << "operator not found: " << opName << "(" << typeName(leftType) << ", " << typeName(rightType) << ")" << std::endl;

        return boost::none;
    }
}

boost::optional<st::Expression> parseTerm(const ast::Term& term, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    if (boost::optional<st::Expression> left = parseFactor(term.first, vts, ft))
    {
        BOOST_FOREACH(const ast::MulOpFactor& mf, term.next)
        {
            if (boost::optional<st::Expression> right = parseFactor(mf.factor, vts, ft))
            {
                left = makeBinaryOperatorCall(operatorName(mf.op), *left, *right,ft);

                if (!left) return boost::none;
            }
            else
            {
                return boost::none;
            }
        }

        return *left;
    }

    return boost::none;
}

boost::optional<st::Expression> parseExpression(const ast::Expression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    if (boost::optional<st::Expression> left = parseTerm(expr.first, vts, ft))
    {
        BOOST_FOREACH(const ast::SignTerm& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseTerm(st.term, vts, ft))
            {
                left = makeBinaryOperatorCall(operatorName(st.sign), *left, *right,ft);

                if (!left) return boost::none;
            }
            else
            {
                return boost::none;
            }
        }

        return *left;
    }

    return boost::none;
}

class ParseStatement : public boost::static_visitor<boost::optional<st::Statement> >
{
public:

    ParseStatement(st::VariableTableStack& vts, const st::FunctionTable& ft) : vts_(&vts), ft_(&ft) { }

    boost::optional<st::Statement> operator()(const ast::CompoundStatement& cs) const
    {
        st::CompoundStatement out;

        vts_->push();

        BOOST_FOREACH(const ast::Statement& s, cs.statements)
        {
            if (boost::optional<st::Statement> st = s.apply_visitor(*this))
            {
                out.statements.push_back(*st);
            }
        }

        vts_->pop();

        return out;
    }

    boost::optional<st::Statement> operator()(const ast::Assignment& assignment) const
    {
        const st::Variable *var = vts_->find(assignment.var);
        boost::optional<st::Expression> expr = parseExpression(assignment.expr, *vts_, *ft_);

        if (!var)
        {
            std::cerr << "unknown identifier: " << assignment.var << std::endl;

            return boost::none;
        }

        if (!expr) return boost::none;

        if (expressionType(*expr) != var->type())
        {
            return st::Assignment(*var, st::Cast(*expr, var->type()));
        }
        else
        {
            return st::Assignment(*var, *expr);
        }
    }

    boost::optional<st::Statement> operator()(const ast::FunctionCall& fc) const
    {
        ParseFactor pf(*vts_, *ft_);

        if (boost::optional<st::Expression> e = pf(fc))
        {
            return boost::get<st::FunctionCall>(*e);
        }
        else
        {
            return boost::none;
        }
    }

    boost::optional<st::Statement> operator()(const ast::ReturnStatement& rs) const
    {
        boost::optional<st::Expression> expr = parseExpression(rs.expr, *vts_, *ft_);

        if (!expr) return boost::none;
        
        return st::ReturnStatement(*expr);
    }

    boost::optional<st::Statement> operator()(const ast::VariableDecl& decl) const
    {
        st::VariableDecl out(decl.name, convertType(decl.type));

        vts_->insert(out.var());

        return out;
    }

private:

    st::VariableTableStack *vts_;
    const st::FunctionTable *ft_;
};

namespace
{

const st::BuiltinFunction operator_plus_ii(operatorName(ast::plus_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_plus_fi(operatorName(ast::plus_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_plus_if(operatorName(ast::plus_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_plus_ff(operatorName(ast::plus_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_minus_ii(operatorName(ast::minus_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_minus_fi(operatorName(ast::minus_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_minus_if(operatorName(ast::minus_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_minus_ff(operatorName(ast::minus_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_mul_ii(operatorName(ast::mul_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_mul_fi(operatorName(ast::mul_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_mul_if(operatorName(ast::mul_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_mul_ff(operatorName(ast::mul_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_div_ii(operatorName(ast::div_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_div_fi(operatorName(ast::div_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_div_if(operatorName(ast::div_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_div_ff(operatorName(ast::div_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_mod_ii(operatorName(ast::mod_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_mod_fi(operatorName(ast::mod_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_mod_if(operatorName(ast::mod_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_mod_ff(operatorName(ast::mod_), st::float_, st::float_, st::float_);

}

void registerBuiltinFunctions(st::FunctionTable& ft)
{
    ft.insert(&operator_plus_ii);
    ft.insert(&operator_plus_fi);
    ft.insert(&operator_plus_if);
    ft.insert(&operator_plus_ff);

    ft.insert(&operator_minus_ii);
    ft.insert(&operator_minus_fi);
    ft.insert(&operator_minus_if);
    ft.insert(&operator_minus_ff);

    ft.insert(&operator_mul_ii);
    ft.insert(&operator_mul_fi);
    ft.insert(&operator_mul_if);
    ft.insert(&operator_mul_ff);

    ft.insert(&operator_div_ii);
    ft.insert(&operator_div_fi);
    ft.insert(&operator_div_if);
    ft.insert(&operator_div_ff);

    ft.insert(&operator_mod_ii);
    ft.insert(&operator_mod_fi);
    ft.insert(&operator_mod_if);
    ft.insert(&operator_mod_ff);
}

st::Module parseModule(const sl::ast::Module& module)
{
    st::Module m(module.name);
    st::FunctionTable functionTable;

    registerBuiltinFunctions(functionTable);

    typedef std::map<st::FunctionDef *, const ast::Function *> FF;
    FF ff;

    // make forward declarations

    BOOST_FOREACH(const ast::Function& f, module.functions)
    {
        st::FunctionDef::ParameterContainer pc;

        BOOST_FOREACH(const ast::FunctionParameter& fp, f.parameters)
        {
            std::auto_ptr<st::Variable> v(new st::Variable(fp.name, convertType(fp.type)));

            pc.push_back(v);
        }

        std::auto_ptr<st::FunctionDef> cf(new st::FunctionDef(f.name, convertType(f.type), std::move(pc)));

        if (!functionTable.insert(&*cf))
        {
            std::cout << "error: function " << f.name << "(";

            BOOST_FOREACH(const ast::FunctionParameter& fp, f.parameters)
            {
                std::cout << st::typeName(convertType(fp.type)) << ", ";
            }

            std::cout << "\b\b) already defined" << std::endl;
        }

        ff.insert(std::make_pair(&*cf, &f));
        m.insertFunction(cf);
    }

    // now, parse the functions

    BOOST_FOREACH(FF::const_reference f, ff)
    {
        st::FunctionDef& df = *f.first;
        const ast::Function& sf = *f.second;

        st::VariableTableStack vts;

        BOOST_FOREACH(const std::shared_ptr<st::Variable>& p, df.parameters())
        {
            vts.insert(*p);
        }

        st::CompoundStatement cs;

        BOOST_FOREACH(const ast::Statement& s, sf.body.statements)
        {
            ParseStatement ps(vts, functionTable);

            if (boost::optional<st::Statement> st = s.apply_visitor(ps))
            {
                cs.statements.push_back(*st);
            }
        }

        df.body(std::move(cs));
    }

    return m;
}

}