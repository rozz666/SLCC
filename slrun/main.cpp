#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

#include "parseAst.hpp"
#include "parseSt.hpp"
#include "codeGen.hpp"


int main(int argc, char **argv)
{
    using namespace sl;
    namespace po = ::boost::program_options;

    std::vector<std::string> inputFiles;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "print version information");

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file", po::value<std::vector<std::string> >(&inputFiles), "input file");

    po::options_description cmdline_options;
    cmdline_options.add(desc).add(hidden);

    po::options_description visible("Allowed options");
    visible.add(desc);
    
    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("version"))
    {                                       
        std::cout << "slrun Copyright (C) 2010 Rafal Przywarski\nVersion 1.0" << std::endl;
        return 1;
    }

    if (!vm.count("input-file"))
    {
        std::cout << "no input" << std::endl;
        return 1;
    }

    BOOST_FOREACH(const std::string& fn, inputFiles)
    {
        std::ifstream fin(fn.c_str());
        boost::optional<ast::Module> module = parseFile(fin);

        if (module)
        {
            std::cout << "parsed " << fn << std::endl;

            sl::st::Module parsed = parseModule(*module);

            std::ofstream fout((fn + "asm").c_str());

            sl::FunctionAddrMap fam;

            sl::vm::BytecodeBuffer bb = generateBytecode(parsed, fam);

            exportToAsm(bb, fout);

            std::ofstream bc((fn + "bin").c_str(), std::ios::binary);
            bc.write((const char *) &bb.front(), bb.size());

        }
        else
        {
            std::cout << "parsing " << fn << " failed" << std::endl;
        }
    }
}