#include <unordered_map>
#include <boost/foreach.hpp>
#include <sl/parseAst.hpp>
#include <sl/builtin.hpp>
#include <sl/err/messages.hpp>

namespace sl
{


ast::Type convertType(cst::Type type)
{
    switch (type)
    {
        case cst::int_: return ast::int_;
        case cst::float_: return ast::float_;
        case cst::bool_: return ast::bool_;
        case cst::void_: return ast::void_;
    }

    assert(!"Invalid type");

    return ast::Type();
}

struct ExpressionType : public boost::static_visitor<ast::Type>
{
    ast::Type operator()(const ast::Constant& c) const
    {
        return c.type();
    }

    ast::Type operator()(const ast::Variable *v) const
    {
        return v->type();
    }

    ast::Type operator()(const ast::FunctionCall& fc) const
    {
        return functionType(fc.f());
    }

    ast::Type operator()(const ast::Cast& c) const
    {
        return c.type();
    }
};

ast::Type expressionType(const ast::Expression& expr)
{
    ExpressionType et;
    return expr.apply_visitor(et);
}

struct ExpressionPos : public boost::static_visitor<FilePosition>
{
    FilePosition operator()(const ast::Constant& c) const
    {
        return c.pos();
    }

    FilePosition operator()(const ast::Variable *v) const
    {
        return v->pos();
    }

    FilePosition operator()(const ast::FunctionCall& fc) const
    {
        return fc.pos();
    }

    FilePosition operator()(const ast::Cast& c) const
    {
        return c.pos();
    }
};

FilePosition expressionPos(const ast::Expression& expr)
{
    ExpressionPos ep;
    return expr.apply_visitor(ep);
}


boost::optional<ast::Expression> parseExpression(const cst::Expression& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger);


boost::optional<ast::FunctionCall> makeUnaryOperatorCall(const FilePosition& pos, const std::string& opName, const ast::Expression& right, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    ast::Type rightType = expressionType(right);

    if (boost::optional<ast::FunctionRef> f = ft.find(ast::functionMangledName(opName, typeSuffix(rightType))))
    {
        return ast::FunctionCall(pos, *f, &right, &right + 1);
    }
    else
    {
        std::ostringstream os;
        os << opName << "(" << typeName(rightType) << ")"; 
        errorLogger << err::function_not_found(pos, os.str());

        return boost::none;
    }
}

class ParseUnaryExpression : public boost::static_visitor<boost::optional<ast::Expression> >
{
public:

    ParseUnaryExpression(ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger) : vts_(vts), ft_(ft), errorLogger_(errorLogger) { }

    boost::optional<ast::Expression> operator()(const cst::Constant& c) const
    {
        return ast::Constant(c.pos, c.value);
    }

    boost::optional<ast::Expression> operator()(const cst::Variable& v) const
    {
        const ast::Variable *pv = vts_.find(v.name.str);

        if (!pv)
        {
            errorLogger_ << err::unknown_identifier(v.name.pos, v.name.str);
            return boost::none;
        }

        return pv;
    }

    boost::optional<ast::Expression> operator()(const cst::FunctionCall& fc) const
    {
        std::vector<ast::Expression> expr;
        std::vector<ast::Type> paramTypes;
        std::string suffix;

        expr.reserve(fc.expr.size());
        paramTypes.reserve(fc.expr.size());

        BOOST_FOREACH(const cst::Expression& e, fc.expr)
        {
            if (boost::optional<ast::Expression> pe = parseExpression(e, vts_, ft_, errorLogger_))
            {
                ast::Type et = expressionType(*pe);
                paramTypes.push_back(et);
                suffix += ast::typeSuffix(et);

                expr.push_back(*pe);
            }
        }

        if (expr.size() < fc.expr.size()) return boost::none;

        boost::optional<ast::FunctionRef> f = ft_.find(ast::functionMangledName(fc.name.str, suffix));

        if (!f)
        {
            std::ostringstream os;
            os << fc.name.str << ast::functionParameters(paramTypes);
            errorLogger_ << err::function_not_found(fc.name.pos, os.str());
            return boost::none;
        }

        ast::FunctionCall out(fc.name.pos, *f, expr.begin(), expr.end());

        return out;
    }

    boost::optional<ast::Expression> operator()(const cst::Expression& e) const
    {
        return parseExpression(e, vts_, ft_, errorLogger_);
    }

    boost::optional<ast::Expression> operator()(const cst::UnOpUnaryExpression& sf) const
    {
        boost::optional<ast::Expression> expr = sf.expr.apply_visitor(*this);

        if (!expr) return boost::none;

        boost::optional<ast::FunctionCall> fc = makeUnaryOperatorCall(sf.opPos, builtin::operatorName(sf.op), *expr, ft_, errorLogger_);

        if (!fc) return boost::none;

        return *fc;
    }

private:

    ast::VariableTableStack& vts_;
    const ast::FunctionTable& ft_;
    ErrorLogger& errorLogger_;
};

boost::optional<ast::Expression> parseUnaryExpression(const cst::UnaryExpression& f, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    ParseUnaryExpression pf(vts, ft, errorLogger);
    return f.apply_visitor(pf);
}

boost::optional<ast::FunctionCall> makeBinaryOperatorCall(const FilePosition& pos, const std::string& opName, const ast::Expression& left, const ast::Expression& right, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    ast::Type leftType = expressionType(left);
    ast::Type rightType = expressionType(right);

    if (boost::optional<ast::FunctionRef> f = ft.find(ast::functionMangledName(opName, std::string(typeSuffix(leftType)) + typeSuffix(rightType))))
    {
        std::vector<ast::Expression> expr;

        expr.reserve(2);
        expr.push_back(left);
        expr.push_back(right);

        return ast::FunctionCall(pos, *f, expr.begin(), expr.end());
    }
    else
    {
        std::ostringstream os;
        os << opName << "(" << typeName(leftType) << ", " << typeName(rightType) << ")";
        errorLogger << err::function_not_found(pos, os.str());

        return boost::none;
    }
}

boost::optional<ast::Expression> parseMultiplicativeExpression(const cst::MultiplicativeExpression& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<ast::Expression> left = parseUnaryExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const cst::MulOpUnaryExpression& mf, expr.next)
        {
            if (boost::optional<ast::Expression> right = parseUnaryExpression(mf.expr, vts, ft, errorLogger))
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

boost::optional<ast::Expression> parseAdditiveExpression(const cst::AdditiveExpression& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<ast::Expression> left = parseMultiplicativeExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const cst::SignMultiplicativeExpression& st, expr.next)
        {
            if (boost::optional<ast::Expression> right = parseMultiplicativeExpression(st.expr, vts, ft, errorLogger))
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

boost::optional<ast::Expression> parseRelationalExpression(const cst::RelationalExpression& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<ast::Expression> left = parseAdditiveExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const cst::RelOpAdditiveExpression& st, expr.next)
        {
            if (boost::optional<ast::Expression> right = parseAdditiveExpression(st.expr, vts, ft, errorLogger))
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

boost::optional<ast::Expression> parseEqualityExpression(const cst::EqualityExpression& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<ast::Expression> left = parseRelationalExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const cst::EqOpRelationalExpression& st, expr.next)
        {
            if (boost::optional<ast::Expression> right = parseRelationalExpression(st.expr, vts, ft, errorLogger))
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

boost::optional<ast::Expression> parseLogicalAndExpression(const cst::LogicalAndExpression& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<ast::Expression> left = parseEqualityExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const cst::LAndOpEqualityExpression& st, expr.next)
        {
            if (boost::optional<ast::Expression> right = parseEqualityExpression(st.expr, vts, ft, errorLogger))
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

boost::optional<ast::Expression> parseExpression(const cst::Expression& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<ast::Expression> left = parseLogicalAndExpression(expr.first, vts, ft, errorLogger))
    {
        BOOST_FOREACH(const cst::LOrOpLogicalAndExpression& st, expr.next)
        {
            if (boost::optional<ast::Expression> right = parseLogicalAndExpression(st.expr, vts, ft, errorLogger))
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

ast::CompoundStatement parseCompoundStatement(const cst::CompoundStatement& cs, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger);

class ParseStatement : public boost::static_visitor<boost::optional<ast::Statement> >
{
public:

    ParseStatement(ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger) : vts_(vts), ft_(ft), errorLogger_(errorLogger) { }

    boost::optional<ast::Statement> operator()(const cst::CompoundStatement& cs) const
    {
        ast::CompoundStatement out;

        vts_.push();

        BOOST_FOREACH(const cst::Statement& s, cs.statements)
        {
            if (boost::optional<ast::Statement> st = s.apply_visitor(*this))
            {
                out.statements.push_back(*st);
            }
        }

        vts_.pop();

        return out;
    }

    boost::optional<ast::Statement> operator()(const cst::Assignment& assignment) const
    {
        const ast::Variable *var = vts_.find(assignment.var.str);
        boost::optional<ast::Expression> expr = parseExpression(assignment.expr, vts_, ft_, errorLogger_);

        if (!var)
        {
            errorLogger_ << err::unknown_identifier(assignment.var.pos, assignment.var.str);
            return boost::none;
        }

        if (!expr) return boost::none;

        ast::Type exprType = expressionType(*expr);

        if (exprType != var->type())
        {
            if (!isConvertible(exprType, var->type()))
            {
                errorLogger_ << err::not_convertible(assignment.expr.firstPos, typeName(exprType), typeName(var->type())); 

                return boost::none;
            }

            return ast::Assignment(*var, ast::Cast(expressionPos(*expr), *expr, var->type()));
        }
        else
        {
            return ast::Assignment(*var, *expr);
        }
    }

    boost::optional<ast::Statement> operator()(const cst::FunctionCall& fc) const
    {
        ParseUnaryExpression pf(vts_, ft_, errorLogger_);

        if (boost::optional<ast::Expression> e = pf(fc))
        {
            return boost::get<ast::FunctionCall>(*e);
        }
        else
        {
            return boost::none;
        }
    }

    boost::optional<ast::Statement> operator()(const cst::ReturnStatement& rs) const
    {
        boost::optional<ast::Expression> expr = parseExpression(rs.expr, vts_, ft_, errorLogger_);

        if (!expr) return boost::none;
        
        return ast::ReturnStatement(*expr);
    }

    boost::optional<ast::Statement> operator()(const cst::IfStatement& ifs) const
    {
        boost::optional<ast::Expression> cond = parseExpression(ifs.cond, vts_, ft_, errorLogger_);

        if (!cond) return boost::none;

        if (expressionType(*cond) != ast::bool_)
        {
            errorLogger_ << err::bool_expr_expected(expressionPos(*cond));
            return boost::none;
        }

        ast::CompoundStatement onTrue = parseCompoundStatement(ifs.onTrue, vts_, ft_, errorLogger_);
        
        if (ifs.onFalse)
        {
            ast::CompoundStatement onFalse = parseCompoundStatement(*ifs.onFalse, vts_, ft_, errorLogger_);

            return ast::IfStatement(std::move(*cond), std::move(onTrue), std::move(onFalse));
        }
        else
        {
            return ast::IfStatement(std::move(*cond), std::move(onTrue));
        }
    }

    boost::optional<ast::Statement> operator()(const cst::WhileLoop& loop) const
    {
        boost::optional<ast::Expression> cond = parseExpression(loop.cond, vts_, ft_, errorLogger_);

        if (!cond) return boost::none;

        if (expressionType(*cond) != ast::bool_)
        {
            errorLogger_ << err::bool_expr_expected(expressionPos(*cond));
            return boost::none;
        }

        ast::CompoundStatement body = parseCompoundStatement(loop.body, vts_, ft_, errorLogger_);

        return ast::WhileLoop(std::move(*cond), std::move(body));
    }

    boost::optional<ast::Statement> operator()(const cst::VariableDecl& decl) const
    {
        if (decl.type)
        {
            if (decl.expr)
            {
                boost::optional<ast::Expression> expr = parseExpression(*decl.expr, vts_, ft_, errorLogger_);

                if (!expr) return boost::none;

                ast::Type type = convertType(*decl.type);
                ast::Type exprType = expressionType(*expr);

                if (!isConvertible(exprType, type))
                {
                    errorLogger_ << err::not_convertible(decl.expr->firstPos, typeName(exprType), typeName(type));
                    return boost::none;
                }

                ast::VariableDecl out(decl.name.str, decl.name.pos, type, type != exprType ? ast::Cast(expressionPos(*expr), *expr, type) : *expr);

                if (!vts_.insert(out.var()))
                {
                    varDeclError(decl.name);
                    return boost::none;
                }

                return out;
            }
            else
            {   
                ast::VariableDecl out(decl.name.str, decl.name.pos, convertType(*decl.type));
                if (!vts_.insert(out.var()))
                {
                    varDeclError(decl.name);
                    return boost::none;
                }

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

            boost::optional<ast::Expression> expr = parseExpression(*decl.expr, vts_, ft_, errorLogger_);

            if (!expr) return boost::none;

            ast::Type exprType = expressionType(*expr);

            if (exprType == ast::void_)
            {
                errorLogger_ << err::void_variable(decl.name.pos, decl.name.str);
                return boost::none;
            }

            ast::VariableDecl out(decl.name.str, decl.name.pos, expressionType(*expr), *expr);
            if (!vts_.insert(out.var()))
            {
                varDeclError(decl.name);
                return boost::none;
            }

            return out;
        }
    }

    boost::optional<ast::Statement> operator()(const cst::VariableDelete& del) const
    {
        if (const ast::Variable *v = vts_.findInScope(del.name.str))
        {
            vts_.eraseInScope(del.name.str);

            return ast::VariableDelete(*v);
        }
        else
        {
            errorLogger_ << err::unknown_identifier(del.name.pos, del.name.str);
        }

        return boost::none;
    }

private:

    ast::VariableTableStack& vts_;
    const ast::FunctionTable& ft_;
    ErrorLogger& errorLogger_;

    void varDeclError(const cst::Identifier& name) const
    {
        errorLogger_ << err::variable_already_declared(name.pos, name.str);
        errorLogger_ << err::variable_earlier_declaration(vts_.findInScope(name.str)->pos(), name.str);
    }
};


ast::CompoundStatement parseCompoundStatement(const cst::CompoundStatement& cs, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    ast::CompoundStatement out;

    ParseStatement ps(vts, ft, errorLogger);

    vts.push();

    BOOST_FOREACH(const cst::Statement& s, cs.statements)
    {
        if (boost::optional<ast::Statement> st = s.apply_visitor(ps))
        {
            out.statements.push_back(*st);
        }
    }

    vts.pop();

    return out;
}

class ParseReturnType : public boost::static_visitor<boost::optional<ast::Type> >
{
public:
    ParseReturnType(ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger) : vts_(vts), ft_(ft), errorLogger_(errorLogger) { }

    boost::optional<ast::Type> operator()(const cst::Type& t) const
    {
        return convertType(t);
    }

    boost::optional<ast::Type> operator()(const cst::Expression& e) const
    {
        boost::optional<ast::Expression> expr = parseExpression(e, vts_, ft_, errorLogger_);

        if (!expr) return boost::none;

        return expressionType(*expr);
    }

private:

    ast::VariableTableStack& vts_;
    const ast::FunctionTable& ft_;
    ErrorLogger& errorLogger_;
};

boost::optional<ast::Type> parseReturnType(const cst::FunctionReturnType frt, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    ParseReturnType prt(vts, ft, errorLogger);
    return frt.apply_visitor(prt);
}

void registerBuiltinFunctions(ast::FunctionTable& ft)
{
    using namespace builtin;

    ft.insert(&operator_plus_i);
    ft.insert(&operator_plus_f);

    ft.insert(&operator_minus_i);
    ft.insert(&operator_minus_f);

    ft.insert(&operator_lnot_b);

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

    ft.insert(&function_geti);
    ft.insert(&function_getf);
    ft.insert(&function_put_i);
    ft.insert(&function_put_f);
}

ast::Module parseModule(const sl::cst::Module& module, ErrorLogger& errorLogger)
{
    ast::Module m(module.name.str);
    ast::FunctionTable functionTable;

    registerBuiltinFunctions(functionTable);

    typedef std::map<ast::FunctionDef *, const cst::Function *> FF;
    FF ff;

    // make forward declarations

    BOOST_FOREACH(const cst::Function& f, module.functions)
    {
        ast::FunctionDef::ParameterContainer pc;
        std::unordered_map<std::string, FilePosition> params;

        BOOST_FOREACH(const cst::FunctionParameter& fp, f.parameters)
        {
            auto p = params.find(fp.name.str);
            if (p == params.end())
            {
                std::shared_ptr<ast::Variable> v(new ast::Variable(fp.name.str, fp.name.pos, convertType(fp.type), fp.ref));

                pc.push_back(v);
                params.insert(std::make_pair(fp.name.str, fp.name.pos));
            }
            else
            {
                errorLogger << err::variable_already_declared(fp.name.pos, fp.name.str);
                errorLogger << err::variable_earlier_declaration(p->second, p->first);
            }
        }

        ast::FunctionDef::ParameterContainer pc2(pc);
        std::auto_ptr<ast::FunctionDef> cf(new ast::FunctionDef(f.name.str, f.name.pos, std::move(pc)));

        if (!functionTable.insert(&*cf))
        {
            std::ostringstream os;
            os << f.name.str << ast::functionParameters(pc2);

            errorLogger << err::function_already_declared(f.name.pos, os.str());
        }

        ff.insert(std::make_pair(&*cf, &f));
        m.insertFunction(cf);
    }

    // parameters and return types

    BOOST_FOREACH(FF::reference f, ff)
    {
        ast::FunctionDef& df = *f.first;

        ast::VariableTableStack vts;

        BOOST_FOREACH(const std::shared_ptr<ast::Variable>& p, df.parameters())
        {
            vts.insert(*p);
        }

        boost::optional<ast::Type> fType = parseReturnType(f.second->type, vts, functionTable, errorLogger);

        if (!fType) return m;

        f.first->type(*fType);
    }


    // now, parse the functions

    BOOST_FOREACH(FF::reference f, ff)
    {
        ast::FunctionDef& df = *f.first;
        const cst::Function& sf = *f.second;

        ast::VariableTableStack vts;

        BOOST_FOREACH(const std::shared_ptr<ast::Variable>& p, df.parameters())
        {
            vts.insert(*p);
        }

        ast::CompoundStatement cs;

        BOOST_FOREACH(const cst::Statement& s, sf.body.statements)
        {
            ParseStatement ps(vts, functionTable, errorLogger);

            if (boost::optional<ast::Statement> st = s.apply_visitor(ps))
            {
                cs.statements.push_back(*st);
            }
        }

        df.body(std::move(cs));
    }

    return m;
}

}