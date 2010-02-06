#include "grammar.hpp"
#include "parseAst.hpp"

namespace sl
{

boost::optional<ast::Module> parseFile(std::istream& is)
{
    typedef std::vector<char> Source;
    Source source;

    source.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    typedef sl::Grammar<Source::const_iterator> Grammar;
    Grammar grammar;
    ast::Module module;

    using boost::spirit::ascii::space;
    Source::const_iterator iter = source.begin();
    Source::const_iterator end = source.end();
    
    bool r = boost::spirit::qi::phrase_parse(iter, end, grammar, space, module);

    if (r && iter == end) return module;
    else return boost::none;
}


}