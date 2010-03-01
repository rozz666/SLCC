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
        case ast::void_: return st::void_;
    }

    assert(!"Invalid type");

    return st::Type();
}

struct ExpressionType : public boost::static_visitor<st::Type>
{
    st::Type operator()(const st::Constant& c) const
    {
        return c.type();
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

struct ExpressionPos : public boost::static_visitor<FilePosition>
{
    FilePosition operator()(const st::Constant& c) const
    {
        return c.pos();
    }

    FilePosition operator()(const st::Variable *v) const
    {
        return v->pos();
    }

    FilePosition operator()(const st::FunctionCall& fc) const
    {
        return fc.pos();
    }

    FilePosition operator()(const st::Cast& c) const
    {
        return c.pos();
    }
};

FilePosition expressionPos(const st::Expression& expr)
{
    ExpressionPos ep;
    return expr.apply_visitor(ep);
}


boost::optional<st::Expression> parseExpression(const ast::Expression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger);


boost::optional<st::FunctionCall> makeUnaryOperatorCall(const FilePosition& pos, const std::string& opName, const st::Expression& right, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    st::Type rightType = expressionType(right);

    if (boost::optional<st::FunctionRef> f = ft.find(st::functionMangledName(opName, typeSuffix(rightType))))
    {
        return st::FunctionCall(pos, *f, &right, &right + 1);
    }
    else
    {
        std::ostringstream os;
        os << opName << "(" << typeName(rightType) << ")"; 
        errorLogger << err::function_not_found(pos, os.str());

        return boost::none;
    }
}

class ParseUnaryExpression : public boost::static_visitor<boost::optional<st::Expression> >
{
public:

    ParseUnaryExpression(st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger) : vts_(vts), ft_(ft), errorLogger_(errorLogger) { }

    boost::optional<st::Expression> operator()(const ast::Constant& c) const
    {
        return st::Constant(c.pos, c.value);
    }

    boost::optional<st::Expression> operator()(const ast::Variable& v) const
    {
        const st::Variable *pv = vts_.find(v.name.str);

        if (!pv)
        {
            errorLogger_ << err::unknown_identifier(v.name.pos, v.name.str);
            return boost::none;
        }

        return pv;
    }

    boost::optional<st::Expression> operator()(const ast::FunctionCall& fc) const
    {
        std::vector<st::Expression> expr;
        std::vector<st::Type> paramTypes;
        std::string suffix;

        expr.reserve(fc.expr.size());
        paramTypes.reserve(fc.expr.size());

        BOOST_FOREACH(const ast::Expression& e, fc.expr)
        {
            if (boost::optional<st::Expression> pe = parseExpression(e, vts_, ft_, errorLogger_))
            {
                st::Type et = expressionType(*pe);
                paramTypes.push_back(et);
                suffix += st::typeSuffix(et);

                expr.push_back(*pe);
            }
        }

        if (expr.size() < fc.expr.size()) return boost::none;

        boost::optional<st::FunctionRef> f = ft_.find(st::functionMangledName(fc.name.str, suffix));

        if (!f)
        {
            std::ostringstream os;
            os << fc.name.str << st::functionParameters(paramTypes);
            errorLogger_ << err::function_not_found(fc.name.pos, os.str());
            return boost::none;
        }

        st::FunctionCall out(fc.name.pos, *f, expr.begin(), expr.end());

        return out;
    }

    boost::optional<st::Expression> operator()(const ast::Expression& e) const
    {
        return parseExpression(e, vts_, ft_, errorLogger_);
    }

    boost::optional<st::Expression> operator()(const ast::SignedUnaryExpression& sf) const
    {
        switch (sf.sign)
        {
            case ast::plus_: return sf.expr.apply_visitor(*this);

            default:

                if (boost::optional<st::Expression> e = sf.expr.apply_visitor(*this))
                {                                                                   
                    if (boost::optional<st::FunctionCall> fc = makeUnaryOperatorCall(sf.signPos, builtin::operatorName(sf.sign), *e, ft_, errorLogger_))
                    {
                        return *fc;
                    }
                }
                
                return boost::none;
        }
    }

private:

    st::VariableTableStack& vts_;
    const st::FunctionTable& ft_;
    ErrorLogger& errorLogger_;
};

boost::optional<st::Expression> parseUnaryExpression(const ast::UnaryExpression& f, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    ParseUnaryExpression pf(vts, ft, errorLogger);
    return f.apply_visitor(pf);
}

boost::optional<st::FunctionCall> makeBinaryOperatorCall(const FilePosition& pos, const std::string& opName, const st::Expression& left, const st::Expression& right, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    st::Type leftType = expressionType(left);
    st::Type rightType = expressionType(right);

    if (boost::optional<st::FunctionRef> f = ft.find(st::functionMangledName(opName, std::string(typeSuffix(leftType)) + typeSuffix(rightType))))
    {
        std::vector<st::Expression> expr;

        expr.reserve(2);
        expr.push_back(left);
        expr.push_back(right);

        return st::FunctionCall(pos, *f, expr.begin(), expr.end());
    }
    else
    {
        std::ostringstream os;
        os << opName << "(" << typeName(leftType) << ", " << typeName(rightType) << ")";
        errorLogger << err::function_not_found(pos, os.str());

        return boost::none;
    }
}

boost::optional<st::Expression> parseMultiplicativeExpression(const ast::MultiplicativeExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<st::Expression> left = parseUnaryExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const ast::MulOpUnaryExpression& mf, expr.next)
        {
            if (boost::optional<st::Expression> right = parseUnaryExpression(mf.expr, vts, ft, errorLogger))
            {
                left = makeBinaryOperatorCall(mf.opPos, builtin::operatorName(mf.op), *left, *right, ft, errorLogger);

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

boost::optional<st::Expression> parseAdditiveExpression(const ast::AdditiveExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<st::Expression> left = parseMultiplicativeExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const ast::SignMultiplicativeExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseMultiplicativeExpression(st.expr, vts, ft, errorLogger))
            {
                left = makeBinaryOperatorCall(st.signPos, builtin::operatorName(st.sign), *left, *right, ft, errorLogger);

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

boost::optional<st::Expression> parseRelationalExpression(const ast::RelationalExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<st::Expression> left = parseAdditiveExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const ast::RelOpAdditiveExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseAdditiveExpression(st.expr, vts, ft, errorLogger))
            {
                left = makeBinaryOperatorCall(st.opPos, builtin::operatorName(st.op), *left, *right, ft, errorLogger);

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

boost::optional<st::Expression> parseEqualityExpression(const ast::EqualityExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<st::Expression> left = parseRelationalExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const ast::EqOpRelationalExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseRelationalExpression(st.expr, vts, ft, errorLogger))
            {
                left = makeBinaryOperatorCall(st.opPos, builtin::operatorName(st.op), *left, *right, ft, errorLogger);

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

boost::optional<st::Expression> parseLogicalAndExpression(const ast::LogicalAndExpression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<st::Expression> left = parseEqualityExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const ast::EqualityExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseEqualityExpression(st, vts, ft, errorLogger))
            {
                // TODO: set pos
                left = makeBinaryOperatorCall(FilePosition(), "operator&&", *left, *right, ft, errorLogger);

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

boost::optional<st::Expression> parseExpression(const ast::Expression& expr, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<st::Expression> left = parseLogicalAndExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const ast::LogicalAndExpression& st, expr.next)
        {
            if (boost::optional<st::Expression> right = parseLogicalAndExpression(st, vts, ft, errorLogger))
            {
                // TODO: set pos
                left = makeBinaryOperatorCall(FilePosition(), "operator||", *left, *right, ft, errorLogger);

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

    ParseStatement(st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger) : vts_(vts), ft_(ft), errorLogger_(errorLogger) { }

    boost::optional<st::Statement> operator()(const ast::CompoundStatement& cs) const
    {
        st::CompoundStatement out;

        vts_.push();

        BOOST_FOREACH(const ast::Statement& s, cs.statements)
        {
            if (boost::optional<st::Statement> st = s.apply_visitor(*this))
            {
                out.statements.push_back(*st);
            }
        }

        vts_.pop();

        return out;
    }

    boost::optional<st::Statement> operator()(const ast::Assignment& assignment) const
    {
        const st::Variable *var = vts_.find(assignment.var.str);
        boost::optional<st::Expression> expr = parseExpression(assignment.expr, vts_, ft_, errorLogger_);

        if (!var)
        {
            errorLogger_ << err::unknown_identifier(assignment.var.pos, assignment.var.str);
            return boost::none;
        }

        if (!expr) return boost::none;

        st::Type exprType = expressionType(*expr);

        if (exprType != var->type())
        {
            if (!isConvertible(exprType, var->type()))
            {
                errorLogger_ << err::not_convertible(assignment.expr.firstPos, typeName(exprType), typeName(var->type())); 

                return boost::none;
            }

            return st::Assignment(*var, st::Cast(expressionPos(*expr), *expr, var->type()));
        }
        else
        {
            return st::Assignment(*var, *expr);
        }
    }

    boost::optional<st::Statement> operator()(const ast::FunctionCall& fc) const
    {
        ParseUnaryExpression pf(vts_, ft_, errorLogger_);

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
        boost::optional<st::Expression> expr = parseExpression(rs.expr, vts_, ft_, errorLogger_);

        if (!expr) return boost::none;
        
        return st::ReturnStatement(*expr);
    }

    boost::optional<st::Statement> operator()(const ast::VariableDecl& decl) const
    {
        if (decl.type)
        {
            if (decl.expr)
            {
                boost::optional<st::Expression> expr = parseExpression(*decl.expr, vts_, ft_, errorLogger_);

                if (!expr) return boost::none;

                st::Type type = convertType(*decl.type);
                st::Type exprType = expressionType(*expr);

                if (!isConvertible(exprType, type))
                {
                    errorLogger_ << err::not_convertible(decl.expr->firstPos, typeName(exprType), typeName(type));
                    return boost::none;
                }

                st::VariableDecl out(decl.name.str, decl.name.pos, type, type != exprType ? st::Cast(expressionPos(*expr), *expr, type) : *expr);
                vts_.insert(out.var());

                return out;
            }
            else
            {   
                st::VariableDecl out(decl.name.str, decl.name.pos, convertType(*decl.type));
                vts_.insert(out.var());

                return out;
            }
        }   
        else
        {
            if (!decl.expr)
            {
                errorLogger_ << err::no_variable_type(decl.name.pos, decl.name.str);
                return boost::none;
            }

            boost::optional<st::Expression> expr = parseExpression(*decl.expr, vts_, ft_, errorLogger_);

            if (!expr) return boost::none;

            st::Type exprType = expressionType(*expr);

            if (exprType == st::void_)
            {
                errorLogger_ << err::void_variable(decl.name.pos, decl.name.str);
                return boost::none;
            }

            st::VariableDecl out(decl.name.str, decl.name.pos, expressionType(*expr), *expr);
            vts_.insert(out.var());

            return out;
        }
    }

    boost::optional<st::Statement> operator()(const ast::VariableDelete& del) const
    {
        if (const st::Variable *v = vts_.findInScope(del.name.str))
        {
            vts_.eraseInScope(del.name.str);

            return st::VariableDelete(*v);
        }
        else
        {
            errorLogger_ << err::unknown_identifier(del.name.pos, del.name.str);
        }

        return boost::none;
    }

private:

    st::VariableTableStack& vts_;
    const st::FunctionTable& ft_;
    ErrorLogger& errorLogger_;
};

class ParseReturnType : public boost::static_visitor<boost::optional<st::Type> >
{
public:
    ParseReturnType(st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger) : vts_(vts), ft_(ft), errorLogger_(errorLogger) { }

    boost::optional<st::Type> operator()(const ast::Type& t) const
    {
        return convertType(t);
    }

    boost::optional<st::Type> operator()(const ast::Expression& e) const
    {
        boost::optional<st::Expression> expr = parseExpression(e, vts_, ft_, errorLogger_);

        if (!expr) return boost::none;

        return expressionType(*expr);
    }

private:

    st::VariableTableStack& vts_;
    const st::FunctionTable& ft_;
    ErrorLogger& errorLogger_;
};

boost::optional<st::Type> parseReturnType(const ast::FunctionReturnType frt, st::VariableTableStack& vts, const st::FunctionTable& ft, ErrorLogger& errorLogger)
{
    ParseReturnType prt(vts, ft, errorLogger);
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

    ft.insert(&function_swap_ii);
    ft.insert(&function_swap_ff);
    ft.insert(&function_swap_bb);
}

st::Module parseModule(const sl::ast::Module& module, ErrorLogger& errorLogger)
{
    st::Module m(module.name.str);
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
            std::auto_ptr<st::Variable> v(new st::Variable(fp.name.str, fp.name.pos, convertType(fp.type), fp.ref));

            pc.push_back(v);
        }

        std::auto_ptr<st::FunctionDef> cf(new st::FunctionDef(f.name.str, f.name.pos, std::move(pc)));

        if (!functionTable.insert(&*cf))
        {
            std::ostringstream os;
            os << f.name.str << st::functionParameters(pc);

            errorLogger << err::function_already_declared(f.name.pos, os.str());
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

        boost::optional<st::Type> fType = parseReturnType(f.second->type, vts, functionTable, errorLogger);

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
            ParseStatement ps(vts, functionTable, errorLogger);

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