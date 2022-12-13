//
// Created by eric on 11.12.22.
//

#ifndef LISP_LVALBUILDINS_H
#define LISP_LVALBUILDINS_H

#include "../environment/ValueHolder.h"
#include <utility>

#include "../parser/Lval.h"
#include "../Constants.h"
#include "buildins_Math.h"
#include "buildins_List.h"
#include "buildins_Misc.h"
#include "Buildins_Function.h"
#include "Buildins_Conditinals.h"

namespace buildins {
    parser::Lval *builtin_eval(parser::ValueHolder *env, parser::Lval *a) {
        LASSERT(a, a->count == 1,
                "Function 'eval' passed too many arguments.");
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'eval' passed incorrect type.");

        parser::Lval *x = a->lval_take(0);
        x->type = parser::LVAL_SEXPR;
        return x->eval(env);
    }

    void lenv_add_builtin(parser::ValueHolder *env, std::string name, parser::lbuiltin func) {
        env->put(parser::Lval::Lval_fun(std::move(name), func));
    }

    void InitializeBuildins(parser::ValueHolder *env) {
        /* List Functions */
        lenv_add_builtin(env, "list", builtin_list);
        lenv_add_builtin(env, "head", builtin_head);
        lenv_add_builtin(env, "tail", builtin_tail);
        lenv_add_builtin(env, "eval", builtin_eval);
        lenv_add_builtin(env, "join", builtin_join);

        /* Mathematical Functions */
        lenv_add_builtin(env, "+", builtin_add);
        lenv_add_builtin(env, "-", builtin_sub);
        lenv_add_builtin(env, "*", builtin_mul);
        lenv_add_builtin(env, "/", builtin_div);

        /* Variable Functions */
        lenv_add_builtin(env, "def", builtin_def);
        lenv_add_builtin(env, "=", builtin_put);
        lenv_add_builtin(env, "\\", builtin_lambda);

        /* Comparison Functions */
        lenv_add_builtin(env, "if", builtin_if);
        lenv_add_builtin(env, "==", builtin_eq);
        lenv_add_builtin(env, "!=", builtin_ne);
        lenv_add_builtin(env, ">", builtin_gt);
        lenv_add_builtin(env, "<", builtin_lt);
        lenv_add_builtin(env, ">=", builtin_ge);
        lenv_add_builtin(env, "<=", builtin_le);

    }
}

#endif //LISP_LVALBUILDINS_H
