#include <tut/tut.hpp> 
#include <sl/parseCst.hpp>
#include <sl/err/messages.hpp>

namespace tut
{

class CodeBuffer
{
public:

	CodeBuffer(std::stringstream& ss) : ss_(ss) { }

	CodeBuffer& operator,(const char *line)
	{
		ss_ << line << "\n";
		return *this;
	}

private:

	std::stringstream& ss_;
};

struct parseCST_Test_data
{
	sl::ErrorLogger el;
	std::stringstream code;
	boost::optional<sl::cst::Module> module;

	parseCST_Test_data() : el("(memory)") { }

	CodeBuffer setCode() { return CodeBuffer(code); }

	void ensure_success()
	{
		ensure("module", module);
		ensure("no errors", el.errors().empty());
	}

	void ensure_failure()
	{
		ensure_not("module", module);
	}

	void ensure_failure(unsigned errorCount)
	{
		ensure_not("module", module);
		ensure_equals("errors", el.errors().size(), errorCount);
	}

	void parse()
	{
		el = sl::ErrorLogger("(memory)");
		module = sl::parseFile(code, el);
	}

	const sl::err::Message& err(unsigned n)
	{
		return el.errors()[n];
	}
};

typedef test_group<parseCST_Test_data> tg;
typedef tg::object object;
tg parseCST_group("Parse CST");

template <>
template <>
void object::test<1>()
{
	set_test_name("empty modules");

	setCode(),
		"module Asia;";

	parse();

	ensure_success();
	ensure_equals("module name 1", module->name.str, "Asia");
	ensure("module name position 1", module->name.pos == sl::FilePosition(1, 8));
	ensure("no declarations 1", module->decls.empty());

	setCode(),
		"   ",
		"        module    ",
		"    Kasia;"
		"    ";

	parse();

	ensure_success();
	ensure_equals("module name 2", module->name.str, "Kasia");
	ensure("module name position 2", module->name.pos == sl::FilePosition(3, 5));
	ensure("no declarations 2", module->decls.empty());
}

template <>
template <>
void object::test<2>()
{
	set_test_name("empty file");

	parse();

	ensure_failure(1);
	ensure("position", err(0).pos == sl::FilePosition(1, 1));
	ensure_equals("error", err(0).text, sl::err::module_declaration_missing(err(0).pos).text);
}

template <>
template <>
void object::test<3>()
{
	set_test_name("function names");

	setCode(),
		"module Asia;",
		"_() -> void { }",
		" a() -> void { }",
		"  a15() -> void { }",
		"   _123135() -> void { }",
		"    aAa() -> void { }",
		"     _64_characters__________________________________________________() -> void { }";

	parse();
	
	ensure_success();
	ensure_equals("6 declarations", module->decls.size(), 6u);
	ensure("function", module->decls[0].type() == typeid(sl::cst::Function));
	ensure("function", module->decls[1].type() == typeid(sl::cst::Function));
	ensure("function", module->decls[2].type() == typeid(sl::cst::Function));
	ensure("function", module->decls[3].type() == typeid(sl::cst::Function));
	ensure("function", module->decls[4].type() == typeid(sl::cst::Function));
	ensure("function", module->decls[5].type() == typeid(sl::cst::Function));
	
	sl::cst::Function& f0 = boost::get<sl::cst::Function>(module->decls[0]);
	sl::cst::Function& f1 = boost::get<sl::cst::Function>(module->decls[1]);
	sl::cst::Function& f2 = boost::get<sl::cst::Function>(module->decls[2]);
	sl::cst::Function& f3 = boost::get<sl::cst::Function>(module->decls[3]);
	sl::cst::Function& f4 = boost::get<sl::cst::Function>(module->decls[4]);
	sl::cst::Function& f5 = boost::get<sl::cst::Function>(module->decls[5]);
	
	ensure("position 0", f0.name.pos == sl::FilePosition(2, 1));
	ensure_equals("name 0", f0.name.str, "_");
	ensure("position 1", f1.name.pos == sl::FilePosition(3, 2));
	ensure_equals("name 1", f1.name.str, "a");
	ensure("position 2", f2.name.pos == sl::FilePosition(4, 3));
	ensure_equals("name 2", f2.name.str, "a15");
	ensure("position 3", f3.name.pos == sl::FilePosition(5, 4));
	ensure_equals("name 3", f3.name.str, "_123135");
	ensure("position 4", f4.name.pos == sl::FilePosition(6, 5));
	ensure_equals("name 4", f4.name.str, "aAa");
	ensure("position 5", f5.name.pos == sl::FilePosition(7, 6));
	ensure_equals("name 5", f5.name.str, "_64_characters__________________________________________________");
}

template <>
template <>
void object::test<4>()
{
	set_test_name("bad function names");

	setCode(),
		"module Asia;",
		"bad+x() -> void { }";

	parse();
	
	ensure_failure(1);

	setCode(),
		"module Asia;",
		"123() -> void { }";

	parse();
	
	ensure_failure(1);
}

/*
template <>
template <>
void object::test<4>()
{
	set_test_name("functions");

	setCode(),
		"module Asia;",
		"f1(int a, ref int b) -> int { }",
		"f2(float a, int b) -> bool { }";

	parse();
	
	ensure_success();
	ensure_equals("2 declaration s", module->decls.size(), 2u);
	ensure("function 1", module->decls[0].type() == typeid(sl::cst::Function));
	ensure("function 2", module->decls[1].type() == typeid(sl::cst::Function));
	
	sl::cst::Function& f1 = boost::get<sl::cst::Function>(module->decls[0]);
	sl::cst::Function& f2 = boost::get<sl::cst::Function>(module->decls[1]);
	
	ensure("position", f1.name.pos == sl::FilePosition(2, 1));
	ensure_equals("name", f1.name.str, "f1");
	ensure_equals("f1 parameters", f1.parameters.size(), 2u);
	//ensure_equals("f1 a", f1.parameters[0].name.pos
	ensure("type", f1.type.type() == typeid(sl::BasicType));
	ensure("f1 int", boost::get<sl::BasicType>(f1.type) == sl::int_);
	ensure("empty body", f1.body.statements.empty());
	//TODO: finish
}

template <>
template <>
void object::test<5>()
{
}
*/
}