#include <iostream>
#include <boost/foreach.hpp>
#include "parseSt.hpp"
#include "builtin.hpp"

namespace sl
{


st::Type convertType(ast::Type type)
{
    switch (type)
    {
        case ast::int_: return st::int_;
        case ast::float_: return st::float_;
        case ast::bool_: return st::bool_;
    }

    assert(!"Invalid type");

    return st::Type();
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

    st::Type operator()(const bool& ) const
    {
        return st::bool_;
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

class ParseUnaryExpression : public boost::static_visitor<boost::optional<st::Expression> >
{
public:

    ParseUnaryExpression(st::VariableTableStack& vts, const st::FunctionTable& ft) : vts_(&vts), ft_(&ft) { }

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

    boost::optional<st::Expression> operator()(const ast::SignedUnaryExpression& sf) const
    {
        switch (sf.sign)
        {
            case ast::plus_: return sf.expr.apply_visitor(*this);

            default:

                if (boost::optional<st::Expression> e = sf.expr.apply_visitor(*this))
                {
                    if (boost::optional<st::FunctionCall> fc = makeUnaryOperatorCall(builtin::operatorName(sf.sign), *e, *ft_))
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

boost::optional<st::Expression> parseUnaryExpression(const ast::UnaryExpression& f, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    ParseUnaryExpression pf(vts, ft);
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

boost::optional<st::Expression> parseMultiplicativeExpression(const ast::MultiplicativeExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    if (boost::optional<st::Expression> left = parseUnaryExpression(expr.first, vts, ft))
    {
        BOOST_FOREACH(const ast::MulOpUnaryExpression& mf, expr.next)
        {
            if (boost::optional<st::Expression> right = parseUnaryExpression(mf.expr, vts, ft))
            {
                left = makeBinaryOperatorCall(builtin::operatorName(mf.op), *left, *right, ft);

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

boost::optional<st::Expression> parseAdditiveExpression(const ast::AdditiveExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    if (boost::optional<st::Expression> left = parseMultiplicativeExpression(expr.first, vts, ft))
    {
        BOOST_FOREACH(const ast::SignMultiplicativeExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseMultiplicativeExpression(st.expr, vts, ft))
            {
                left = makeBinaryOperatorCall(builtin::operatorName(st.sign), *left, *right, ft);

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

boost::optional<st::Expression> parseRelationalExpression(const ast::RelationalExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    if (boost::optional<st::Expression> left = parseAdditiveExpression(expr.first, vts, ft))
    {
        BOOST_FOREACH(const ast::RelOpAdditiveExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseAdditiveExpression(st.expr, vts, ft))
            {
                left = makeBinaryOperatorCall(builtin::operatorName(st.op), *left, *right, ft);

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

boost::optional<st::Expression> parseEqualityExpression(const ast::EqualityExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    if (boost::optional<st::Expression> left = parseRelationalExpression(expr.first, vts, ft))
    {
        BOOST_FOREACH(const ast::EqOpRelationalExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseRelationalExpression(st.expr, vts, ft))
            {
                left = makeBinaryOperatorCall(builtin::operatorName(st.op), *left, *right, ft);

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

boost::optional<st::Expression> parseLogicalAndExpression(const ast::LogicalAndExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    if (boost::optional<st::Expression> left = parseEqualityExpression(expr.first, vts, ft))
    {
        BOOST_FOREACH(const ast::EqualityExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseEqualityExpression(st, vts, ft))
            {
                left = makeBinaryOperatorCall("operator&&", *left, *right, ft);

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
    if (boost::optional<st::Expression> left = parseLogicalAndExpression(expr.first, vts, ft))
    {
        BOOST_FOREACH(const ast::LogicalAndExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseLogicalAndExpression(st, vts, ft))
            {
                left = makeBinaryOperatorCall("operator||", *left, *right, ft);

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

        st::Type exprType = expressionType(*expr);

        if (exprType != var->type())
        {
            if (!isConvertible(exprType, var->type())) return boost::none;

            return st::Assignment(*var, st::Cast(*expr, var->type()));
        }
        else
        {
            return st::Assignment(*var, *expr);
        }
    }

    boost::optional<st::Statement> operator()(const ast::FunctionCall& fc) const
    {
        ParseUnaryExpression pf(*vts_, *ft_);

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
        if (decl.type)
        {
            if (decl.expr)
            {
                boost::optional<st::Expression> expr = parseExpression(*decl.expr, *vts_, *ft_);

                if (!expr) return boost::none;

                st::Type type = convertType(*decl.type);

                st::Type exprType = expressionType(*expr);

                if (!isConvertible(exprType, type)) return boost::none;

                st::VariableDecl out(decl.name, type, type != exprType ? st::Cast(*expr, type) : *expr);
                vts_->insert(out.var());

                return out;
            }
            else
            {   
                st::VariableDecl out(decl.name, convertType(*decl.type));
                vts_->insert(out.var());

                return out;
            }
        }   
        else
        {
            if (!decl.expr) return boost::none;

            boost::optional<st::Expression> expr = parseExpression(*decl.expr, *vts_, *ft_);

            if (!expr) return boost::none;

            st::VariableDecl out(decl.name, expressionType(*expr), *expr);
            vts_->insert(out.var());

            return out;
        }
    }

    boost::optional<st::Statement> operator()(const ast::VariableDelete& del) const
    {
        if (const st::Variable *v = vts_->findInScope(del.name))
        {
            vts_->eraseInScope(del.name);

            return st::VariableDelete(*v);
        }
        else
        {
            std::cerr << "Unknown identifier " << del.name << std::endl;
        }

        return boost::none;
    }

private:

    st::VariableTableStack *vts_;
    const st::FunctionTable *ft_;
};

class ParseReturnType : public boost::static_visitor<boost::optional<st::Type> >
{
public:
    ParseReturnType(st::VariableTableStack& vts, const st::FunctionTable& ft) : vts_(vts), ft_(ft) { }

    boost::optional<st::Type> operator()(const ast::Type& t) const
    {
        return convertType(t);
    }

    boost::optional<st::Type> operator()(const ast::Expression& e) const
    {
        boost::optional<st::Expression> expr = parseExpression(e, vts_, ft_);

        if (!expr) return boost::none;

        return expressionType(*expr);
    }

private:

    st::VariableTableStack& vts_;
    const st::FunctionTable& ft_;
};

boost::optional<st::Type> parseReturnType(const ast::FunctionReturnType frt, st::VariableTableStack& vts, const st::FunctionTable& ft)
{
    ParseReturnType prt(vts, ft);
    return frt.apply_visitor(prt);
}

void registerBuiltinFunctions(st::FunctionTable& ft)
{
    using namespace builtin;

    ft.insert(&operator_minus_i);
    ft.insert(&operator_minus_f);

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

    ft.insert(&operator_lt_ii);
    ft.insert(&operator_lt_fi);
    ft.insert(&operator_lt_if);
    ft.insert(&operator_lt_ff);

    ft.insert(&operator_le_ii);
    ft.insert(&operator_le_fi);
    ft.insert(&operator_le_if);
    ft.insert(&operator_le_ff);

    ft.insert(&operator_gt_ii);
    ft.insert(&operator_gt_fi);
    ft.insert(&operator_gt_if);
    ft.insert(&operator_gt_ff);

    ft.insert(&operator_ge_ii);
    ft.insert(&operator_ge_fi);
    ft.insert(&operator_ge_if);
    ft.insert(&operator_ge_ff);

    ft.insert(&operator_eq_ii);
    ft.insert(&operator_eq_fi);
    ft.insert(&operator_eq_if);
    ft.insert(&operator_eq_ff);
    ft.insert(&operator_eq_bb);

    ft.insert(&operator_neq_ii);
    ft.insert(&operator_neq_fi);
    ft.insert(&operator_neq_if);
    ft.insert(&operator_neq_ff);
    ft.insert(&operator_neq_bb);

    ft.insert(&operator_land_bb);
    ft.insert(&operator_lor_bb);
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
            std::auto_ptr<st::Variable> v(new st::Variable(fp.name, convertType(fp.type), fp.ref));

            pc.push_back(v);
        }

        std::auto_ptr<st::FunctionDef> cf(new st::FunctionDef(f.name, std::move(pc)));

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

    // parameters and return types

    BOOST_FOREACH(FF::reference f, ff)
    {
        st::FunctionDef& df = *f.first;

        st::VariableTableStack vts;

        BOOST_FOREACH(const std::shared_ptr<st::Variable>& p, df.parameters())
        {
            vts.insert(*p);
        }

        boost::optional<st::Type> fType = parseReturnType(f.second->type, vts, functionTable);

        if (!fType) return m;

        f.first->type(*fType);
    }


    // now, parse the functions

    BOOST_FOREACH(FF::reference f, ff)
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