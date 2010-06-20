#ifndef SL_BUILTIN_HPP
#define SL_BUILTIN_HPP

#include <sl/cst.hpp>
#include <sl/ast.hpp>

namespace sl
{

namespace builtin
{

const char *operatorName(cst::UnOp s);
const char *operatorName(cst::MulOp o);
const char *operatorName(cst::RelOp o);
const char *operatorName(cst::EqOp o);
const char *operatorName(cst::LAndOp o);
const char *operatorName(cst::LOrOp o);

extern const ast::BuiltinFunction operator_plus_i;
extern const ast::BuiltinFunction operator_plus_f;
extern const ast::BuiltinFunction operator_minus_i;
extern const ast::BuiltinFunction operator_minus_f;
extern const ast::BuiltinFunction operator_lnot_b;

extern const ast::BuiltinFunction operator_plus_ii;
extern const ast::BuiltinFunction operator_plus_fi;
extern const ast::BuiltinFunction operator_plus_if;
extern const ast::BuiltinFunction operator_plus_ff;

extern const ast::BuiltinFunction operator_minus_ii;
extern const ast::BuiltinFunction operator_minus_fi;
extern const ast::BuiltinFunction operator_minus_if;
extern const ast::BuiltinFunction operator_minus_ff;

extern const ast::BuiltinFunction operator_mul_ii;
extern const ast::BuiltinFunction operator_mul_fi;
extern const ast::BuiltinFunction operator_mul_if;
extern const ast::BuiltinFunction operator_mul_ff;

extern const ast::BuiltinFunction operator_div_ii;
extern const ast::BuiltinFunction operator_div_fi;
extern const ast::BuiltinFunction operator_div_if;
extern const ast::BuiltinFunction operator_div_ff;

extern const ast::BuiltinFunction operator_mod_ii;
extern const ast::BuiltinFunction operator_mod_fi;
extern const ast::BuiltinFunction operator_mod_if;
extern const ast::BuiltinFunction operator_mod_ff;

extern const ast::BuiltinFunction operator_lt_ii;
extern const ast::BuiltinFunction operator_lt_fi;
extern const ast::BuiltinFunction operator_lt_if;
extern const ast::BuiltinFunction operator_lt_ff;

extern const ast::BuiltinFunction operator_le_ii;
extern const ast::BuiltinFunction operator_le_fi;
extern const ast::BuiltinFunction operator_le_if;
extern const ast::BuiltinFunction operator_le_ff;

extern const ast::BuiltinFunction operator_gt_ii;
extern const ast::BuiltinFunction operator_gt_fi;
extern const ast::BuiltinFunction operator_gt_if;
extern const ast::BuiltinFunction operator_gt_ff;

extern const ast::BuiltinFunction operator_ge_ii;
extern const ast::BuiltinFunction operator_ge_fi;
extern const ast::BuiltinFunction operator_ge_if;
extern const ast::BuiltinFunction operator_ge_ff;

extern const ast::BuiltinFunction operator_eq_ii;
extern const ast::BuiltinFunction operator_eq_fi;
extern const ast::BuiltinFunction operator_eq_if;
extern const ast::BuiltinFunction operator_eq_ff;
extern const ast::BuiltinFunction operator_eq_bb;

extern const ast::BuiltinFunction operator_neq_ii;
extern const ast::BuiltinFunction operator_neq_fi;
extern const ast::BuiltinFunction operator_neq_if;
extern const ast::BuiltinFunction operator_neq_ff;
extern const ast::BuiltinFunction operator_neq_bb;

extern const ast::BuiltinFunction operator_land_bb;
extern const ast::BuiltinFunction operator_lor_bb;

extern const ast::BuiltinFunction function_swap_ii;
extern const ast::BuiltinFunction function_swap_ff;
extern const ast::BuiltinFunction function_swap_bb;

extern const ast::BuiltinFunction function_geti;
extern const ast::BuiltinFunction function_getf;
extern const ast::BuiltinFunction function_put_i;
extern const ast::BuiltinFunction function_put_f;

}
}

#endif /* SL_BUILTIN_HPP */