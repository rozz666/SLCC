#ifndef SL_BUILTIN_HPP
#define SL_BUILTIN_HPP

#include "ast.hpp"
#include "st.hpp"

namespace sl
{

namespace builtin
{

const char *operatorName(ast::UnOp s);
const char *operatorName(ast::MulOp o);
const char *operatorName(ast::RelOp o);
const char *operatorName(ast::EqOp o);
const char *operatorName(ast::LAndOp o);
const char *operatorName(ast::LOrOp o);

extern const st::BuiltinFunction operator_plus_i;
extern const st::BuiltinFunction operator_plus_f;
extern const st::BuiltinFunction operator_minus_i;
extern const st::BuiltinFunction operator_minus_f;
extern const st::BuiltinFunction operator_lnot_b;

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

extern const st::BuiltinFunction operator_lt_ii;
extern const st::BuiltinFunction operator_lt_fi;
extern const st::BuiltinFunction operator_lt_if;
extern const st::BuiltinFunction operator_lt_ff;

extern const st::BuiltinFunction operator_le_ii;
extern const st::BuiltinFunction operator_le_fi;
extern const st::BuiltinFunction operator_le_if;
extern const st::BuiltinFunction operator_le_ff;

extern const st::BuiltinFunction operator_gt_ii;
extern const st::BuiltinFunction operator_gt_fi;
extern const st::BuiltinFunction operator_gt_if;
extern const st::BuiltinFunction operator_gt_ff;

extern const st::BuiltinFunction operator_ge_ii;
extern const st::BuiltinFunction operator_ge_fi;
extern const st::BuiltinFunction operator_ge_if;
extern const st::BuiltinFunction operator_ge_ff;

extern const st::BuiltinFunction operator_eq_ii;
extern const st::BuiltinFunction operator_eq_fi;
extern const st::BuiltinFunction operator_eq_if;
extern const st::BuiltinFunction operator_eq_ff;
extern const st::BuiltinFunction operator_eq_bb;

extern const st::BuiltinFunction operator_neq_ii;
extern const st::BuiltinFunction operator_neq_fi;
extern const st::BuiltinFunction operator_neq_if;
extern const st::BuiltinFunction operator_neq_ff;
extern const st::BuiltinFunction operator_neq_bb;

extern const st::BuiltinFunction operator_land_bb;
extern const st::BuiltinFunction operator_lor_bb;

extern const st::BuiltinFunction function_swap_ii;
extern const st::BuiltinFunction function_swap_ff;
extern const st::BuiltinFunction function_swap_bb;

extern const st::BuiltinFunction function_geti;
extern const st::BuiltinFunction function_getf;
extern const st::BuiltinFunction function_put_i;
extern const st::BuiltinFunction function_put_f;

}
}

#endif /* SL_BUILTIN_HPP */