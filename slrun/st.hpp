#ifndef SL_ST_HPP
#define SL_ST_HPP

#include <iostream>
#include <string>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/foreach.hpp>

namespace sl
{
namespace st
{

enum Type
{
    int_,
    float_
};

inline const char *typeSuffix(Type type)
{
    switch (type)
    {
        case int_: return "i";
        case float_: return "f";
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
    }

    assert(!"Invalid type");

    return "";
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

typedef boost::variant<int, float> Constant;

class Variable
{
public:

    Variable(const std::string& name, Type type) : name_(name), type_(type) { }

    const std::string& name() const { return name_; }
    Type type() const { return type_; }

private:

    std::string name_;
    Type type_;
};

class FunctionCall;
class Cast;

typedef boost::variant<
    Constant,
    const Variable *,
    boost::recursive_wrapper<FunctionCall>,
    boost::recursive_wrapper<Cast>
> Expression;

struct CompoundStatement;

class Cast
{
public:

    Cast(const Expression& expr, Type type) : expr_(expr), type_(type) { }

    const Expression& expr() const { return expr_; }
    Type type() const { return type_; }

private:

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

    VariableDecl(const std::string& name, Type type) : var_(new Variable(name, type)) { }

    Variable& var() { return *var_; }
    const Variable& var() const { return *var_; }

private:

    std::shared_ptr<Variable> var_;
};

typedef boost::variant<
    boost::recursive_wrapper<CompoundStatement>, 
    Assignment, 
    FunctionCall, 
    ReturnStatement,
    VariableDecl
> Statement;

struct CompoundStatement
{
    std::vector<Statement> statements;
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
   
    FunctionDef(const std::string& name, Type type, ParameterContainer&& parameters) 
        : name_(name), suffix_(functionSuffix(parameters)), type_(type)
    {
        parameters.swap(parameters_);
    }

    const std::string& name() const { return name_; }
    const std::string& suffix() const { return suffix_; }
    Type type() const { return type_; }

    const ParameterContainer& parameters() const { return parameters_; }

    const boost::optional<CompoundStatement>& body() const { return body_; }

    void body(CompoundStatement&& c)
    {
        body_ = c;
    }

private:

    std::string name_;
    std::string suffix_;     
    Type type_;
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
    FunctionCall(FunctionRef f, It firstParam, It lastParam)
        : f_(f), params_(firstParam, lastParam) { }

    const FunctionRef& f() const { return f_; }

    const ParamContainer& params() const { return params_; }

private:

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