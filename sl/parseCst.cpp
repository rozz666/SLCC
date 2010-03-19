#include <sl/grammar.hpp>
#include <sl/parseCst.hpp>

namespace sl
{

boost::optional<cst::Module> parseFile(std::istream& is, ErrorLogger& errorLogger)
{
    typedef std::vector<char> Source;
    Source source;

    source.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    typedef boost::spirit::classic::position_iterator<Source::const_iterator, boost::spirit::classic::file_position> PosIterator;

    using boost::spirit::ascii::space;
    PosIterator iter(source.begin(), source.end());
    PosIterator end;

    typedef sl::Grammar<PosIterator> Grammar;
    Grammar grammar(errorLogger);
    cst::Module module;

    bool r = boost::spirit::qi::phrase_parse(iter, end, grammar, space, module);

    if (r && iter == end) return module;
    else return boost::none;
}


}