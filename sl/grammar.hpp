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
#include <boost/spirit/home/classic/iterator/position_iterator.hpp>

#include <sl/ErrorLogger.hpp>
#include <sl/err/messages.hpp>
#include <sl/cst.hpp>
/*
namespace boost
{
namespace spirit
{
namespace traits
{
    
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

    static void fail(Exposed& )
    {
    }
};

}
}
}
*/
namespace sl
{
    BOOST_SPIRIT_TERMINAL(inputPos);
}

namespace boost
{
namespace spirit 
{ 
    template <>
    struct use_terminal<qi::domain, sl::tag::inputPos> : mpl::true_  { }; 
}
}

namespace sl
{
    struct inputPosParser : boost::spirit::qi::primitive_parser<inputPosParser>
    {
        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef sl::FilePosition type;
        };

        template <typename Iterator, typename Context, typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last, Context&, Skipper const& skipper, Attribute& attr) const
        {
            boost::spirit::qi::skip_over(first, last, skipper);
            boost::spirit::traits::assign_to(sl::FilePosition(first.get_position().file, first.get_position().line, first.get_position().column), attr);
            return true;
        }

        template <typename Context>
        boost::spirit::info what(Context&) const
        {
            return boost::spirit::info("inputPos");
        }
    };
}

namespace boost
{
namespace spirit
{
namespace qi
{
    template <typename Modifiers>
    struct make_primitive<sl::tag::inputPos, Modifiers>
    {
        typedef sl::inputPosParser result_type;

        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
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

struct Type : qi::symbols<char, BasicType>
{
    Type()
    {
        add
            ("int", int_)
            ("float", float_)
            ("bool", bool_);
    }
};

struct ReturnType : qi::symbols<char, BasicType>
{
    ReturnType()
    {
        add
            ("int", int_)
            ("float", float_)
            ("bool", bool_)
            ("void", void_);
    }
};

struct UnOp : qi::symbols<char, cst::UnOp>
{
    UnOp()
    {
        add
            ("+", cst::plus_)
            ("-", cst::minus_)
            ("!", cst::lnot_);
    }
};

struct Sign : qi::symbols<char, cst::UnOp>
{
    Sign()
    {
        add
            ("+", cst::plus_)
            ("-", cst::minus_);
    }
};

struct MulOp : qi::symbols<char, cst::MulOp>
{
    MulOp()
    {
        add
            ("*", cst::mul_)
            ("/", cst::div_)
            ("%", cst::mod_);
    }
};

struct RelOp : qi::symbols<char, cst::RelOp>
{
    RelOp()
    {
        add
            ("<", cst::less_)
            ("<=", cst::lessEqual_)
            (">", cst::greater_)
            (">=", cst::greaterEqual_);
    }
};

struct EqOp : qi::symbols<char, cst::EqOp>
{
    EqOp()
    {
        add
            ("==", cst::equal_)
            ("!=", cst::notEqual_);
    }
};

struct LAndOp : qi::symbols<char, cst::LAndOp>
{
    LAndOp()
    {
        add("&&", cst::and_);
    }
};

struct LOrOp : qi::symbols<char, cst::LOrOp>
{
    LOrOp()
    {
        add("||", cst::or_);
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

struct errorMessageImpl
{
    template <typename Logger, typename String, typename It, typename What>
    struct result { typedef void type; };

    template <typename Logger, typename String, typename It, typename What>
    void operator()(Logger *logger, String filename, It it, What what) const
    {
        std::ostringstream os;
        os << what;
        *logger << err::syntax_error(FilePosition(filename, it.get_position().line, it.get_position().column), os.str());
    }
};

boost::phoenix::function<errorMessageImpl> errorMessage;



template <typename Iterator>
struct Grammar : qi::grammar<Iterator, cst::Module(), ascii::space_type> 
{
    Grammar(ErrorLogger& errorLogger, const std::string& filename)
        : Grammar::base_type(module, "module"), errorLogger_(&errorLogger), filename_(filename)
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
        using boost::phoenix::ref;
        using boost::phoenix::at_c;
        using boost::phoenix::push_back;
        using namespace qi::labels;            

        identifier %= inputPos >> lexeme[char_("a-zA-Z_") >> *char_("a-zA-Z0-9_")];
        constant %= inputPos >> ((int_ >> !char_('.')) | float_ | boolLit);
        unaryExpression %= constant | functionCall | identifier | ('(' >> expression >> ')') | unOpUnaryExpression;
        unOpUnaryExpression %= inputPos >> unOp >> inputPos >> unaryExpression;
        multiplicativeExpression %= inputPos >> unaryExpression >> *(inputPos >> mulOp >> inputPos >> unaryExpression);
        additiveExpression %= inputPos >> multiplicativeExpression >> *(inputPos >> sign >> inputPos >> multiplicativeExpression);
        relationalExpression %= inputPos >> additiveExpression >> *(inputPos >> relOp >> inputPos >> additiveExpression);
        equalityExpression %= inputPos >> relationalExpression >> *(inputPos >> eqOp >> inputPos >> relationalExpression); 
        andExpression = inputPos >> equalityExpression >> *(inputPos >> landOp >> inputPos >> equalityExpression); 
        expression = inputPos >> andExpression >> *(inputPos >> lorOp >> inputPos >> andExpression); 
        variableDecl %= "new" > -type > identifier > -('=' > expression) > ';';
        variableDelete %= "delete" > identifier > ';';
        functionParameter %= (lit("ref") >> attr(true) | attr(false)) >> type >> identifier;
        assignment = identifier >> '=' >> expression;
        functionCall %= identifier >> '(' >> -(expression % ',') > ')';
        returnStatement %= "return" > -expression > ';';
        ifStatement = lit("if") > '(' > expression > ')' > compoundStatement > -(lit("else") > compoundStatement);
        whileLoop = lit("while") > '(' > expression > ')' > compoundStatement;
        statement %=
            compoundStatement |
            returnStatement |
            variableDecl |
            variableDelete |
            ifStatement |
            whileLoop |
            (assignment > ';') |
            (functionCall > ';');
        compoundStatement = inputPos >> '{' >> *statement >> inputPos > '}';
        function %= identifier >> '(' >> -(functionParameter % ',') >> ')' >> "->" >> (returnType | (lit("typeof") >> '(' >> expression >> ')')) >> compoundStatement;
        import %= lit("import") >> identifier > ';';
        globalDecl %= import | function;
        module %= "module" > identifier > ';' >> *globalDecl >> qi::eoi;

        identifier.name("identifier");
        constant.name("constant");
        unaryExpression.name("unaryExpression");
        unOpUnaryExpression.name("unOpUnaryExpression");
        multiplicativeExpression.name("multiplicativeExpression");
        expression.name("expression");
        variableDecl.name("variable declaration");
        functionParameter.name("function parameter");
        assignment.name("assignment");
        functionCall.name("function call");
        returnStatement.name("return statement");
        ifStatement.name("if statement");
        whileLoop.name("while loop");
        statement.name("statement");
        compoundStatement.name("compound statement");
        function.name("function definition");
        import.name("import");
        globalDecl.name("global declaration");
        module.name("module body");

        on_error<fail>
        (
            module,
            errorMessage(errorLogger_, filename_, _3, _4)
        );
    }

    ErrorLogger *errorLogger_;
    std::string filename_;
    detail::Type type;
    detail::ReturnType returnType;
    detail::UnOp unOp;
    detail::Sign sign;
    detail::MulOp mulOp;
    detail::RelOp relOp;
    detail::EqOp eqOp;
    detail::LAndOp landOp;
    detail::LOrOp lorOp;
    detail::BoolLit boolLit;
    qi::rule<Iterator, cst::Identifier(), ascii::space_type> identifier;
    qi::rule<Iterator, cst::Constant(), ascii::space_type> constant;
    qi::rule<Iterator, cst::UnaryExpression(), ascii::space_type> unaryExpression;
    qi::rule<Iterator, cst::UnOpUnaryExpression(), ascii::space_type> unOpUnaryExpression;
    qi::rule<Iterator, cst::MultiplicativeExpression(), ascii::space_type> multiplicativeExpression;
    qi::rule<Iterator, cst::AdditiveExpression(), ascii::space_type> additiveExpression;
    qi::rule<Iterator, cst::RelationalExpression(), ascii::space_type> relationalExpression;
    qi::rule<Iterator, cst::EqualityExpression(), ascii::space_type> equalityExpression;
    qi::rule<Iterator, cst::LogicalAndExpression(), ascii::space_type> andExpression;
    qi::rule<Iterator, cst::Expression(), ascii::space_type> expression;
    qi::rule<Iterator, cst::VariableDecl(), ascii::space_type> variableDecl;
    qi::rule<Iterator, cst::VariableDelete(), ascii::space_type> variableDelete;
    qi::rule<Iterator, cst::FunctionParameter(), ascii::space_type> functionParameter;
    qi::rule<Iterator, cst::Assignment(), ascii::space_type> assignment;
    qi::rule<Iterator, cst::FunctionCall(), ascii::space_type> functionCall;
    qi::rule<Iterator, cst::ReturnStatement(), ascii::space_type> returnStatement;
    qi::rule<Iterator, cst::IfStatement(), ascii::space_type> ifStatement;
    qi::rule<Iterator, cst::WhileLoop(), ascii::space_type> whileLoop;
    qi::rule<Iterator, cst::Statement(), ascii::space_type> statement;
    qi::rule<Iterator, cst::CompoundStatement(), ascii::space_type> compoundStatement;
    qi::rule<Iterator, cst::Function(), ascii::space_type> function;
    qi::rule<Iterator, cst::Import(), ascii::space_type> import;
    qi::rule<Iterator, cst::GlobalDecl(), ascii::space_type> globalDecl;
    qi::rule<Iterator, cst::Module(), ascii::space_type> module;
};

}

#endif /* SL_GRAMMAR_HPP */