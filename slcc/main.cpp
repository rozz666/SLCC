#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

#include <sl/parseCst.hpp>
#include <sl/parseAst.hpp>
#include <sl/codeGen.hpp>


int main(int argc, char **argv)
{
    using namespace sl;
    namespace po = ::boost::program_options;

    std::vector<std::string> inputFiles;

    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("version", "print version information")
        ("asm,a", "generate assembler");

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file", po::value<std::vector<std::string> >(&inputFiles), "input file");

    po::options_description cmdline_options;
    cmdline_options.add(desc).add(hidden);

    po::options_description visible("Options");
    visible.add(desc);
    
    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << "Usage: slcc [options] file..." << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("version"))
    {                                       
        std::cout << "slcc 1.0\nCopyright (C) 2010 Rafal Przywarski" << std::endl;
        return 1;
    }

    if (!vm.count("input-file"))
    {
        std::cout << "no input" << std::endl;
        return 1;
    }

    bool genAsm = vm.count("asm") != 0;

    BOOST_FOREACH(const std::string& fn, inputFiles)
    {
        ErrorLogger errorLogger;
        std::ifstream fin(fn.c_str());

        if (!fin.is_open())
        {
            std::cerr << fn << " not found" << std::endl;
            continue;
        }

        boost::optional<cst::Module> module = parseFile(fn, fin, errorLogger);

        if (module)
        {
            std::cout << "parsed " << fn << std::endl;

            sl::ast::Module parsed = parseModule(*module, errorLogger);

            if (errorLogger.errors().empty())
            {
                sl::FunctionAddrMap fam;

                sl::vm::BytecodeBuffer bb = generateBytecode(parsed, fam);

                if (genAsm)
                {
                    std::ofstream fout((fn + "asm").c_str());
                    exportToAsm(bb, fout);
                }

                std::ofstream bc((fn + "bin").c_str(), std::ios::binary);
                bc.write((const char *) &bb.front(), bb.size());
            }
            else
            {
                errorLogger.print(std::cerr);
            }
        }
        else
        {
            std::cout << "parsing " << fn << " failed" << std::endl;
            errorLogger.print(std::cerr);
        }
    }
}