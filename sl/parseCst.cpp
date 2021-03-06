#include <sl/grammar.hpp>
#include <sl/parseCst.hpp>

namespace sl
{

boost::optional<cst::Module> parseFile(const std::string& filename, std::istream& is, ErrorLogger& errorLogger)
{
    typedef std::vector<char> Source;
    Source source;

    source.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    typedef boost::spirit::classic::position_iterator<Source::const_iterator, boost::spirit::classic::file_position> PosIterator;

    using boost::spirit::ascii::space;
    PosIterator iter(source.begin(), source.end(), filename);
    PosIterator end;

    typedef sl::Grammar<PosIterator> Grammar;
    Grammar grammar(errorLogger, filename);
    cst::Module module;

    bool r = boost::spirit::qi::phrase_parse(iter, end, grammar, space, module);

    if (r && iter == end) return module;
    else
    {
        if (errorLogger.errors().empty())
        {
            errorLogger << err::module_declaration_missing(FilePosition(filename, iter.get_position().line, iter.get_position().column));
        }

        return boost::none;
    }
}


}