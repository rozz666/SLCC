#include <unordered_set>
#include <unordered_map>
#include <boost/foreach.hpp>
#include <boost/array.hpp>
#include <sl/basicTypes.hpp>
#include <sl/parseAst.hpp>
#include <sl/builtin.hpp>
#include <sl/err/messages.hpp>

namespace sl
{

struct ExpressionType : public boost::static_visitor<BasicType>
{
    BasicType operator()(const ast::Constant& c) const
    {
        return c.type();
    }

    BasicType operator()(const ast::Variable& v) const
    {
        return v.type();
    }

    BasicType operator()(const ast::FunctionCall& fc) const
    {
        return functionType(fc.f());
    }

    BasicType operator()(const ast::Cast& c) const
    {
        return c.type();
    }
};

BasicType expressionType(const ast::Expression& expr)
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

    FilePosition operator()(const ast::Variable& v) const
    {
        return v.pos();
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


template <typename ExpressionType>
boost::optional<ast::Expression> parseExpression(const ExpressionType& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger);


boost::optional<ast::FunctionCall> makeUnaryOperatorCall(const FilePosition& pos, const std::string& opName, const ast::Expression& right, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    BasicType rightType = expressionType(right);
    boost::array<sl::BasicType, 1> argTypes = { rightType };

    std::vector<ast::FunctionRef> f = ft.find(opName, argTypes);

    if (f.empty())
    {
        std::ostringstream os;
        os << opName << "(" << ast::typeName(rightType) << ")"; 
        errorLogger << err::function_not_found(pos, os.str());

        return boost::none;
    }

    if (f.size() > 1)
    {
        std::ostringstream os;
        os << opName << "(" << ast::typeName(rightType) << ")"; 
        errorLogger << err::ambiguous_call_to_function(pos, os.str());

        return boost::none;
    }

    return ast::FunctionCall(pos, f[0], &right, &right + 1);
}

class ParseUnaryExpression : public boost::static_visitor<boost::optional<ast::Expression> >
{
public:

    ParseUnaryExpression(ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger) : vts_(vts), ft_(ft), errorLogger_(errorLogger) { }

    boost::optional<ast::Expression> operator()(const cst::Constant& c) const
    {
        return ast::Constant(c.pos, c.value);
    }

    boost::optional<ast::Expression> operator()(const cst::Identifier& v) const
    {
        boost::optional<ast::Variable> var = vts_.find(v.str);

        if (!var)
        {
            errorLogger_ << err::unknown_identifier(v.pos, v.str);
            return boost::none;
        }

        return *var;
    }

    boost::optional<ast::Expression> operator()(const cst::FunctionCall& fc) const
    {
        std::vector<ast::Expression> expr;
        std::vector<BasicType> paramTypes;

        expr.reserve(fc.expr.size());
        paramTypes.reserve(fc.expr.size());

        BOOST_FOREACH(const cst::Expression& e, fc.expr)
        {
            if (boost::optional<ast::Expression> pe = parseExpression(e, vts_, ft_, errorLogger_))
            {
                paramTypes.push_back(expressionType(*pe));

                expr.push_back(*pe);
            }
        }

        if (expr.size() < fc.expr.size()) return boost::none;

        std::vector<ast::FunctionRef> f = ft_.find(fc.name.str, paramTypes);

        if (f.empty())
        {
            std::ostringstream os;
            os << fc.name.str << ast::strParameters(paramTypes);
            errorLogger_ << err::function_not_found(fc.name.pos, os.str());
            return boost::none;
        }
        
        if (f.size() > 1)
        {
            std::ostringstream os;
            os << fc.name.str << ast::strParameters(paramTypes);
            errorLogger_ << err::ambiguous_call_to_function(fc.name.pos, os.str());
            return boost::none;
        }

        ast::FunctionCall out(fc.name.pos, f[0], expr.begin(), expr.end());

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

boost::optional<ast::FunctionCall> makeBinaryOperatorCall(const FilePosition& pos, const std::string& opName, const ast::Expression& left, const ast::Expression& right, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    BasicType leftType = expressionType(left);
    BasicType rightType = expressionType(right);
    boost::array<sl::BasicType, 2> argTypes = { leftType, rightType };

    std::vector<ast::FunctionRef> f = ft.find(opName, argTypes);

    if (f.empty())
    {
        std::ostringstream os;
        os << opName << "(" << ast::typeSuffix(leftType) << ", " << ast::typeSuffix(rightType) << ")";
        errorLogger << err::function_not_found(pos, os.str());

        return boost::none;
    }

    if (f.size() > 1)
    {
        std::ostringstream os;
        os << opName << "(" << ast::typeSuffix(leftType) << ", " << ast::typeSuffix(rightType) << ")";
        errorLogger << err::ambiguous_call_to_function(pos, os.str());

        return boost::none;
    }

    std::vector<ast::Expression> expr;

    expr.reserve(2);
    expr.push_back(left);
    expr.push_back(right);

    return ast::FunctionCall(pos, f[0], expr.begin(), expr.end());
}

boost::optional<ast::Expression> parseExpression(const cst::UnaryExpression& f, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    ParseUnaryExpression pf(vts, ft, errorLogger);
    return f.apply_visitor(pf);
}

template <typename ExpressionType>
boost::optional<ast::Expression> parseExpression(const ExpressionType& expr, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
{
    if (boost::optional<ast::Expression> left = parseExpression(expr.first, vts, ft, errorLogger))
    {                           
        for (auto it = expr.next.begin(); it != expr.next.end(); ++it)
        {
            if (boost::optional<ast::Expression> right = parseExpression(it->expr, vts, ft, errorLogger))
            {
                left = makeBinaryOperatorCall(it->opPos, builtin::operatorName(it->op), *left, *right, ft, errorLogger);

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
        boost::optional<ast::Variable> var = vts_.find(assignment.var.str);
        boost::optional<ast::Expression> expr = parseExpression(assignment.expr, vts_, ft_, errorLogger_);

        if (!var)
        {
            errorLogger_ << err::unknown_identifier(assignment.var.pos, assignment.var.str);
            return boost::none;
        }

        if (!expr) return boost::none;

        BasicType exprType = expressionType(*expr);

        if (exprType != var->type())
        {
            if (!ast::isConvertible(exprType, var->type()))
            {
                errorLogger_ << err::not_convertible(assignment.expr.firstPos, ast::typeSuffix(exprType), ast::typeSuffix(var->type())); 

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

        if (expressionType(*cond) != bool_)
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

        if (expressionType(*cond) != bool_)
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

                BasicType type = *decl.type;
                BasicType exprType = expressionType(*expr);

                if (!ast::isConvertible(exprType, type))
                {
                    errorLogger_ << err::not_convertible(decl.expr->firstPos, ast::typeSuffix(exprType), ast::typeSuffix(type));
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
                ast::VariableDecl out(decl.name.str, decl.name.pos, *decl.type);
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

            BasicType exprType = expressionType(*expr);

            if (exprType == void_)
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
        boost::optional<ast::Variable> v = vts_.findInScope(del.name.str);

        if (!v)
        {
            errorLogger_ << err::unknown_identifier(del.name.pos, del.name.str);
            return boost::none;
        }

        vts_.eraseInScope(del.name.str);

        return ast::VariableDelete(*v);
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

class ParseReturnType : public boost::static_visitor<boost::optional<BasicType> >
{
public:
    ParseReturnType(ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger) : vts_(vts), ft_(ft), errorLogger_(errorLogger) { }

    boost::optional<BasicType> operator()(const BasicType& t) const
    {
        return t;
    }

    boost::optional<BasicType> operator()(const cst::Expression& e) const
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

boost::optional<BasicType> parseReturnType(const cst::FunctionReturnType frt, ast::VariableTableStack& vts, const ast::FunctionTable& ft, ErrorLogger& errorLogger)
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

typedef std::unordered_map<ast::FunctionDef *, const cst::Function *> FF;

class ParseGlobalDecl : public boost::static_visitor<>
{
public:

    ParseGlobalDecl(ast::Module& m, FF& ff, ErrorLogger& errorLogger, ast::FunctionTable& ft) : m_(m), ff_(ff), errorLogger_(errorLogger), ft_(ft) { }

    void operator()(const cst::Function& f)
    {
        ast::FunctionDef::ParameterContainer pc;
        std::unordered_map<std::string, FilePosition> params;

        BOOST_FOREACH(const cst::FunctionParameter& fp, f.parameters)
        {
            auto p = params.find(fp.name.str);

            if (p == params.end())
            {
                ast::Variable v(fp.name.str, fp.name.pos, fp.type, fp.ref);

                pc.push_back(v);
                params.insert(std::make_pair(fp.name.str, fp.name.pos));
            }
            else
            {
                errorLogger_ << err::variable_already_declared(fp.name.pos, fp.name.str);
                errorLogger_ << err::variable_earlier_declaration(p->second, p->first);
            }
        }

        ast::FunctionDef::ParameterContainer pc2(pc);
        std::auto_ptr<ast::FunctionDef> cf(new ast::FunctionDef(f.name.str, f.name.pos, std::move(pc)));

        if (!ft_.insert(&*cf))
        {
            std::ostringstream os;
            os << f.name.str << ast::strParameters(pc2);

            errorLogger_ << err::function_already_declared(f.name.pos, os.str());
            
            ast::FunctionRef fr = *ft_.findExact(cf->name(), functionArgTypes(*cf));
            
            if (fr.type() == typeid(const ast::FunctionDef *))
            {
                errorLogger_ << err::function_earlier_declaration(boost::get<const ast::FunctionDef *>(fr)->pos(), os.str());
            }
        }

        ff_.insert(std::make_pair(&*cf, &f));
        m_.insertFunction(cf);
    }

    void operator()(const cst::Import& ) { } // ignore

private:

    ast::Module& m_;
    FF& ff_;
    ErrorLogger& errorLogger_;
    ast::FunctionTable& ft_;
};

void parseGlobalDecl(const sl::cst::GlobalDecl& gd, ast::Module& m, FF& ff, ErrorLogger& errorLogger, ast::FunctionTable& ft)
{
    ParseGlobalDecl pgd(m, ff, errorLogger, ft);
    gd.apply_visitor(pgd);
}

ast::Module parseModule(const sl::cst::Module& module, ErrorLogger& errorLogger)
{
    ast::Module m(module.name.str);
    ast::FunctionTable functionTable;

    registerBuiltinFunctions(functionTable);

    FF ff;

    // make forward declarations

    BOOST_FOREACH(const cst::GlobalDecl& gd, module.decls)
    {
        parseGlobalDecl(gd, m, ff, errorLogger, functionTable);
    }

    // parameters and return types

    BOOST_FOREACH(FF::reference f, ff)
    {
        ast::FunctionDef& df = *f.first;

        ast::VariableTableStack vts;

        BOOST_FOREACH(const ast::Variable& p, df.parameters())
        {
            vts.insert(p);
        }

        boost::optional<BasicType> fType = parseReturnType(f.second->type, vts, functionTable, errorLogger);

        if (!fType) return m;

        f.first->type(*fType);
    }


    // now, parse the functions

    BOOST_FOREACH(FF::reference f, ff)
    {
        ast::FunctionDef& df = *f.first;
        const cst::Function& sf = *f.second;

        ast::VariableTableStack vts;

        BOOST_FOREACH(const ast::Variable& p, df.parameters())
        {
            vts.insert(p);
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

    boost::optional<ast::FunctionRef> mainFunc = functionTable.findExact("main", std::vector<sl::BasicType>());

    if (!mainFunc)
    {
        errorLogger << err::missing_main(FilePosition(module.name.pos.file, 1, 1));
        return m;
    }

    const ast::FunctionDef& mainDef = *boost::get<const ast::FunctionDef *>(*mainFunc);

    if (mainDef.type() != void_)
    {
        errorLogger << err::invalid_main_type(mainDef.pos());
        return m;
    }

    return m;
}

class ParseImport;

bool parseModules(const std::string& name, ast::ModuleMap& mm, ModuleLoader loader, ErrorLogger& errorLogger, ParseImport& pi);

class ParseImport : public boost::static_visitor<bool>
{
public:

    ParseImport(ast::ModuleMap& mm, ModuleLoader loader, ErrorLogger& errorLogger) : mm_(mm), loader_(loader), errorLogger_(errorLogger) { }

    bool operator()(const cst::Function& ) { return true; } // ignore

    bool operator()(const cst::Import& import)
    {
        if (tried_.find(import.module.str) != tried_.end()) return true;

        tried_.insert(import.module.str);

        return parseModules(import.module.str, mm_, loader_, errorLogger_, *this);
    }

private:

    std::unordered_set<std::string> tried_;
    ast::ModuleMap& mm_;
    ModuleLoader loader_;
    ErrorLogger& errorLogger_;
};

bool parseModules(const std::string& name, ast::ModuleMap& mm, ModuleLoader loader, ErrorLogger& errorLogger, ParseImport& pi)
{
    boost::optional<cst::Module> module = loader(name, errorLogger);

    if (!module) return false;

    BOOST_FOREACH(const cst::GlobalDecl& gd, module->decls)
    {
        if (!gd.apply_visitor(pi)) return false;
    }

    mm.insert(name, std::move(*module));

    return true;
}

bool parseModules(const std::string& name, ast::ModuleMap& mm, ModuleLoader loader, ErrorLogger& errorLogger)
{
    ParseImport pi(mm, loader, errorLogger);

    return parseModules(name, mm, loader, errorLogger, pi);
}

}