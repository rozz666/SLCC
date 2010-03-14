#ifndef SL_ST_HPP
#define SL_ST_HPP

#include <string>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/foreach.hpp>
#include "FilePosition.hpp"

namespace sl
{
namespace st
{

enum Type
{
    int_,
    float_,
    bool_,
    void_
};

inline const char *typeSuffix(Type type)
{
    switch (type)
    {
        case int_: return "i";
        case float_: return "f";
        case bool_: return "b";
    }

    assert(!"Invalid type");

    return "";
}

inline const char *typeName(Type type)
{
    switch (type)
    {
        case int_: return "int";
        case float_: return "float";
        case bool_: return "bool";
    }

    assert(!"Invalid type");

    return "";
}

inline bool isConvertible(Type from, Type to)
{
    return
        (from == to) ||
        (to == float_ && from == int_) ||
        (to == int_ && from == float_);
}

class Variable;

template <typename Container>
inline 
typename boost::enable_if<
    boost::is_same<typename boost::remove_reference<typename Container::reference>::type, std::shared_ptr<Variable> >,
    std::string
>::type
functionSuffix(const Container& params)
{
    std::string suffix;

    BOOST_FOREACH(const std::shared_ptr<Variable>& v, params)
    {
        suffix += typeSuffix(v->type());
    }

    return suffix;
}

template <typename Container>
inline 
typename boost::enable_if<
    boost::is_same<typename boost::remove_reference<typename Container::reference>::type, std::shared_ptr<Variable> >,
    std::string
>::type
functionParameters(const Container& params)
{
    std::ostringstream os;
    bool comma = false;

    os << "(";

    BOOST_FOREACH(const std::shared_ptr<Variable>& v, params)
    {
        if (comma) os << ", ";
        comma = true;
        os << typeName(v->type());
    }

    os << ")";

    return os.str();
}

template <typename Container>
inline 
typename boost::enable_if<
    boost::is_same<typename boost::remove_reference<typename Container::reference>::type, st::Type>,
    std::string
>::type
functionParameters(const Container& params)
{
    std::ostringstream os;
    bool comma = false;

    os << "(";

    BOOST_FOREACH(st::Type v, params)
    {
        if (comma) os << ", ";
        comma = true;
        os << typeName(v);
    }

    os << ")";

    return os.str();
}

inline std::string functionMangledName(const std::string& name, const std::string& suffix)
{
    return name + "$" + suffix;
}

template <typename Container>
inline
typename boost::enable_if<
    boost::is_same<typename boost::remove_reference<typename Container::reference>::type, Variable>,
    std::string
>::type
functionMangledName(const std::string& name, const Container& params)
{
    return functionMangledName(name, functionSuffix(params));
}

struct ConstantType : public boost::static_visitor<Type>
{
    Type operator()(const int& ) const
    {
        return st::int_;
    }

    Type operator()(const float& ) const
    {
        return st::float_;
    }

    Type operator()(const bool& ) const
    {
        return st::bool_;
    }
};

class Constant
{
public:

    typedef boost::variant<int, float, bool> Value;

    Constant(const FilePosition& pos, const Value& value) : pos_(pos), value_(value) { }

    FilePosition pos() const { return pos_; }
    Value value() const { return value_; }
    Type type() const
    {
        ConstantType ct;
        return value_.apply_visitor(ct);
    }

private:

    FilePosition pos_;
    Value value_;
};

class Variable
{
public:

    Variable(const std::string& name, const FilePosition& pos, Type type, bool ref) : name_(name), pos_(pos), type_(type), ref_(ref) { }

    const std::string& name() const { return name_; }
    FilePosition pos() const { return pos_; }
    Type type() const { return type_; }
    bool ref() const { return ref_; }

private:

    std::string name_;
    FilePosition pos_;
    Type type_;
    bool ref_;
};

class FunctionCall;
class Cast;

typedef boost::variant<
    Constant,
    const Variable *,
    boost::recursive_wrapper<FunctionCall>,
    boost::recursive_wrapper<Cast>
> Expression;


struct IsLValue : public boost::static_visitor<bool>
{
    bool operator()(const Variable *) const { return true; }

    template <typename T>
    bool operator()(const T& ) const { return false; }
};

inline bool isLValue(const Expression& e)
{
    IsLValue v;
    return e.apply_visitor(v);
}

struct CompoundStatement;
class IfStatement;

class Cast
{
public:

    Cast(const FilePosition& pos, const Expression& expr, Type type) : pos_(pos), expr_(expr), type_(type) { }

    FilePosition pos() const { return pos_; }
    const Expression& expr() const { return expr_; }
    Type type() const { return type_; }

private:

    FilePosition pos_;
    Expression expr_;
    Type type_;
};

class Assignment
{
public:

    Assignment(const Variable& var, const Expression& expr) : var_(&var), expr_(expr) { }

    const Variable& var() const { return *var_; }
    const Expression& expr() const { return expr_; }

private:

    const Variable *var_;
    Expression expr_;
};

class ReturnStatement
{
public:

    ReturnStatement(const Expression& expr) : expr_(expr) { }

    const Expression& expr() const { return expr_; }

private:

    Expression expr_;
};

class VariableDecl  
{
public:

    VariableDecl(const std::string& name, const FilePosition& pos, Type type) : var_(new Variable(name, pos, type, false)) { }
    VariableDecl(const std::string& name, const FilePosition& pos, Type type, const Expression& expr) : var_(new Variable(name, pos, type, false)), expr_(expr) { }

    Variable& var() { return *var_; }
    const Variable& var() const { return *var_; }

    const boost::optional<Expression>& expr() const { return expr_; }

private:

    std::shared_ptr<Variable> var_;
    boost::optional<Expression> expr_;
};

class VariableDelete
{
public:

    VariableDelete(const Variable& var) : var_(&var) { }

    const Variable& var() const { return *var_; }

private:

    const Variable *var_;
};

typedef boost::variant<
    boost::recursive_wrapper<CompoundStatement>, 
    Assignment, 
    FunctionCall, 
    ReturnStatement,
    IfStatement,
    VariableDecl,
    VariableDelete
> Statement;

struct CompoundStatement
{
    std::vector<Statement> statements;
};

class IfStatement
{
public:

    IfStatement(Expression&& cond, CompoundStatement&& onTrue) : cond_(cond), onTrue_(onTrue) { }
    IfStatement(Expression&& cond, CompoundStatement&& onTrue, CompoundStatement&& onFalse) : cond_(cond), onTrue_(onTrue), onFalse_(onFalse) { }

    const Expression& cond() const { return cond_; }
    const CompoundStatement& onTrue() const { return onTrue_; }
    const boost::optional<CompoundStatement>& onFalse() const { return onFalse_; }

private:

    Expression cond_;
    CompoundStatement onTrue_;
    boost::optional<CompoundStatement> onFalse_;
};

class BuiltinFunction
{
public:

    BuiltinFunction(const std::string& name, Type type) : name_(name), type_(type) { }

    BuiltinFunction(const std::string& name, Type arg0, Type type) : name_(name), suffix_(typeSuffix(arg0)), type_(type) { }

    BuiltinFunction(const std::string& name, Type arg0, Type arg1, Type type) : name_(name), type_(type)
    {
        suffix_ = typeSuffix(arg0);
        suffix_ += typeSuffix(arg1);
    }

    const std::string& name() const { return name_; }
    const std::string& suffix() const { return suffix_; }
    Type type() const { return type_; }

private:

    std::string name_;
    std::string suffix_;     
    Type type_;
};

class FunctionDef
{
public:

    typedef std::vector<std::shared_ptr<Variable> > ParameterContainer;
   
    FunctionDef(const std::string& name, const FilePosition& pos, ParameterContainer&& parameters) 
        : name_(name), suffix_(functionSuffix(parameters)), pos_(pos)
    {
        parameters.swap(parameters_);
    }

    const std::string& name() const { return name_; }
    const std::string& suffix() const { return suffix_; }
    FilePosition pos() const { return pos_; }
    Type type() const { return *type_; }
    void type(st::Type val) { type_ = val; }

    const ParameterContainer& parameters() const { return parameters_; }

    const boost::optional<CompoundStatement>& body() const { return body_; }

    void body(CompoundStatement&& c)
    {
        body_ = c;
    }

private:

    std::string name_;
    std::string suffix_;
    FilePosition pos_;
    boost::optional<Type> type_;
    ParameterContainer parameters_;
    boost::optional<CompoundStatement> body_;
};

inline std::string functionMangledName(const FunctionDef& f)
{
    return functionMangledName(f.name(), f.suffix());
}

typedef boost::variant<
    const BuiltinFunction *,
    const FunctionDef *
> FunctionRef;

struct FunctionMangledName : public boost::static_visitor<std::string>
{
    template <typename T>
    std::string operator()(const T& f) const { return st::functionMangledName(f->name(), f->suffix()); }
};

inline std::string functionMangledName(const FunctionRef& f)
{
    FunctionMangledName fmn;
    return f.apply_visitor(fmn);
}

struct FunctionType : public boost::static_visitor<Type>
{
    template <typename T>
    Type operator()(const T& f) const { return f->type(); }
};

inline Type functionType(const FunctionRef& f)
{
    FunctionType ft;
    return f.apply_visitor(ft);
}

class FunctionCall
{
public:

    typedef std::vector<Expression> ParamContainer;

    template <typename It>    
    FunctionCall(const FilePosition& pos, FunctionRef f, It firstParam, It lastParam)
        : pos_(pos), f_(f), params_(firstParam, lastParam) { }

    FilePosition pos() const { return pos_; }
    const FunctionRef& f() const { return f_; }
    const ParamContainer& params() const { return params_; }

private:

    FilePosition pos_;
    FunctionRef f_;
    ParamContainer params_;
};


class FunctionTable
{
    typedef std::map<std::string, FunctionRef> C;

public:

    FunctionTable() { }

    bool insert(const FunctionRef& f)
    {
        return functions_.insert(std::make_pair(functionMangledName(f), f)).second;
    }

    boost::optional<FunctionRef> find(const std::string& name) const
    {
        C::const_iterator it = functions_.find(name);

        if (it != functions_.end()) return it->second;
        else return boost::none;
    }

private:

    C functions_;
};

class VariableTable
{
    typedef std::map<std::string, const Variable *> C; 

public:

    bool insert(const Variable& var)
    {
        return vars_.insert(std::make_pair(var.name(), &var)).second;
    }

    const Variable *find(const std::string& name) const
    {
        C::const_iterator it = vars_.find(name);

        return (it != vars_.end()) ? it->second : nullptr;
    }

    void erase(const std::string& name)
    {
        vars_.erase(name);        
    }

private:

    C vars_;
};

class VariableTableStack
{
    typedef std::vector<VariableTable> C;

public:

    VariableTableStack()
    {
        push();
    }

    bool insert(const Variable& var)
    {
        return stack_.back().insert(var);
    }

    const Variable *find(const std::string& name) const
    {
        for (C::const_reverse_iterator it = stack_.rbegin(); it != stack_.rend(); ++it)
        {
            if (const Variable *v = it->find(name)) return v;
        }

        return nullptr;
    }

    const Variable *findInScope(const std::string& name) const
    {
        return stack_.back().find(name);
    }

    void eraseInScope(const std::string& name)
    {
        stack_.back().erase(name);
    }

    void push()
    {
        stack_.push_back(VariableTable());
    }

    void pop()
    {
        stack_.pop_back();
    }

private:

    C stack_;
};

class Module : boost::noncopyable
{
public:

    typedef boost::ptr_vector<FunctionDef> FunctionContainer;

    Module(Module&& right)
    {
        name_.swap(right.name_);
        functions_.swap(right.functions_);
    }

    Module(const std::string& name) : name_(name) { }

    void insertFunction(std::auto_ptr<FunctionDef> function)
    {
        functions_.push_back(function);
    }

    const std::string name() const { return name_; }

    const FunctionContainer& functions() const { return functions_; }

private:

    std::string name_;
    FunctionContainer functions_;
};

}
}

#endif /* SL_ST_HPP */