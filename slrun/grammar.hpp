#ifndef SL_GRAMMAR_HPP
#define SL_GRAMMAR_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/tuple/tuple.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "ast.hpp"

namespace boost { namespace spirit { namespace traits {

    
template <typename Exposed, typename Transformed>
struct transform_attribute<
    Exposed,
    Transformed,
    typename enable_if_c<
    fusion::result_of::size<typename enable_if<fusion::traits::is_sequence<Exposed>, Exposed>::type>::type::value == 1 &&
        is_convertible<
            typename fusion::result_of::at_c<Exposed, 0>::type,
            Transformed
        >::value &&
        is_convertible<
            Transformed,
            typename fusion::result_of::at_c<Exposed, 0>::type
        >::value 
    >::type
>
{
    typedef Transformed type;

    static Transformed pre(Exposed const& val)
    {
        return boost::fusion::at_c<0>(val);
    }                                         

    static void post(Exposed& val, Transformed const& attr)
    {
        boost::fusion::at_c<0>(val) = attr;
    }
};

}
}
}

namespace sl
{

namespace qi = ::boost::spirit::qi;
namespace ascii = ::boost::spirit::ascii;

namespace detail
{

struct Type : qi::symbols<char, ast::Type>
{
    Type()
    {
        add
            ("int", ast::int_)
            ("float", ast::float_)
            ("bool", ast::bool_);
    }
};

struct Sign : qi::symbols<char, ast::Sign>
{
    Sign()
    {
        add
            ("+", ast::plus_)
            ("-", ast::minus_);
    }
};

struct MulOp : qi::symbols<char, ast::MulOp>
{
    MulOp()
    {
        add
            ("*", ast::mul_)
            ("/", ast::div_)
            ("%", ast::mod_);
    }
};

struct BoolLit : qi::symbols<char, bool>
{
    BoolLit()
    {
        add
            ("true", true)
            ("false", false);
    }
};

}

struct position_impl
{
    template <typename It1, typename It2>
    struct result { typedef std::string type; };

    template <typename It1, typename It2>
    std::string operator()(It1 first, It2 pos) const
    {
        unsigned line = 1, col = 1;
        for (; first != pos; ++first)
        {
            if (*first == '\n')
            {
                ++line;
                col = 1;
            }
            else ++col;
        }

        std::ostringstream ss;

        ss << "(" << line << ", " << col << ")";

        return ss.str();
    }
};

boost::phoenix::function<position_impl> position;

template <typename Iterator>
struct Grammar : qi::grammar<Iterator, ast::Module(), ascii::space_type> 
{
    Grammar() : Grammar::base_type(module, "module")
    {
        using qi::int_;
        using qi::float_;
        using qi::char_;
        using qi::lit;
        using qi::attr;
        using qi::lexeme;
        using qi::on_error;
        using qi::fail;
        using boost::phoenix::construct;
        using boost::phoenix::val;
        using boost::phoenix::at_c;
        using boost::phoenix::push_back;
        using namespace qi::labels;            

        identifier %= lexeme[char_("a-zA-Z_") >> *char_("a-zA-Z0-9_")];
        constant %= (int_ >> !char_('.')) | float_ | boolLit;
        variable %= identifier;
        factor %= constant | functionCall | variable | ('(' >> expression >> ')') | signedFactor;
        signedFactor = sign >> factor;
        term = factor >> *(mulOp > factor);
        expression = term >> *(sign > term);
        variableDecl = "new" > -type > identifier > -('=' > expression) > ';';
        variableDelete = "delete" > identifier > ';';
        functionParameter = (lit("ref") >> attr(true) | attr(false)) >> type >> identifier;
        assignment = identifier >> '=' > expression;
        functionCall = identifier >> '(' >> -(expression % ',') > ')';
        returnStatement = "return" > expression > ';';
        statement =
            compoundStatement |
            returnStatement |
            (assignment > ';') |
            (functionCall > ';') |
            variableDecl |
            variableDelete;
        compoundStatement = '{' >> (*statement)[_val = _1] > '}';
        function = identifier >> '(' >> -(functionParameter % ',') >> ')' >> "->" >> (type | (lit("typeof") >> '(' >> expression >> ')')) >> compoundStatement;
        module = "module" > identifier > ';' >> *function >> qi::eoi;

        identifier.name("identifier");
        constant.name("constant");
        variable.name("variable");
        factor.name("factor");
        signedFactor.name("signedFactor");
        term.name("term");
        expression.name("expression");
        variableDecl.name("variable declaration");
        functionParameter.name("function parameter");
        assignment.name("assignment");
        functionCall.name("function call");
        returnStatement.name("return statement");
        statement.name("statement");
        compoundStatement.name("compound statement");
        function.name("function definition");
        module.name("module body");

        on_error<fail>
        (
            module,
            std::cout
                << val("Error") << position(_1, _3) << val(": expected ")
                << _4                               // what failed?
                << std::endl
        );
    }

    detail::Type type;
    detail::Sign sign;
    detail::MulOp mulOp;
    detail::BoolLit boolLit;
    qi::rule<Iterator, std::string(), ascii::space_type> identifier;
    qi::rule<Iterator, ast::Constant(), ascii::space_type> constant;
    qi::rule<Iterator, ast::Variable(), ascii::space_type> variable;
    qi::rule<Iterator, ast::Factor(), ascii::space_type> factor;
    qi::rule<Iterator, ast::SignedFactor(), ascii::space_type> signedFactor;
    qi::rule<Iterator, ast::Term(), ascii::space_type> term;
    qi::rule<Iterator, ast::Expression(), ascii::space_type> expression;
    qi::rule<Iterator, ast::VariableDecl(), ascii::space_type> variableDecl;
    qi::rule<Iterator, ast::VariableDelete(), ascii::space_type> variableDelete;
    qi::rule<Iterator, ast::FunctionParameter(), ascii::space_type> functionParameter;
    qi::rule<Iterator, ast::Assignment(), ascii::space_type> assignment;
    qi::rule<Iterator, ast::FunctionCall(), ascii::space_type> functionCall;
    qi::rule<Iterator, ast::ReturnStatement(), ascii::space_type> returnStatement;
    qi::rule<Iterator, ast::Statement(), ascii::space_type> statement;
    qi::rule<Iterator, ast::CompoundStatement(), ascii::space_type> compoundStatement;
    qi::rule<Iterator, ast::Function(), ascii::space_type> function;
    qi::rule<Iterator, ast::Module(), ascii::space_type> module;
};

}

#endif /* SL_GRAMMAR_HPP */