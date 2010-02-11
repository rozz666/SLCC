#ifndef SL_BUILTIN_HPP
#define SL_BUILTIN_HPP

#include "ast.hpp"
#include "st.hpp"

namespace sl
{

namespace builtin
{

const char *operatorName(ast::Sign s);
const char *operatorName(ast::MulOp o);

extern const st::BuiltinFunction operator_minus_i;
extern const st::BuiltinFunction operator_minus_f;

extern const st::BuiltinFunction operator_plus_ii;
extern const st::BuiltinFunction operator_plus_fi;
extern const st::BuiltinFunction operator_plus_if;
extern const st::BuiltinFunction operator_plus_ff;

extern const st::BuiltinFunction operator_minus_ii;
extern const st::BuiltinFunction operator_minus_fi;
extern const st::BuiltinFunction operator_minus_if;
extern const st::BuiltinFunction operator_minus_ff;

extern const st::BuiltinFunction operator_mul_ii;
extern const st::BuiltinFunction operator_mul_fi;
extern const st::BuiltinFunction operator_mul_if;
extern const st::BuiltinFunction operator_mul_ff;

extern const st::BuiltinFunction operator_div_ii;
extern const st::BuiltinFunction operator_div_fi;
extern const st::BuiltinFunction operator_div_if;
extern const st::BuiltinFunction operator_div_ff;

extern const st::BuiltinFunction operator_mod_ii;
extern const st::BuiltinFunction operator_mod_fi;
extern const st::BuiltinFunction operator_mod_if;
extern const st::BuiltinFunction operator_mod_ff;


}
}

#endif /* SL_BUILTIN_HPP */