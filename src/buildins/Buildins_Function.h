//
// Created by eric on 12.12.22.
//

#ifndef LISP_BUILDINS_FUNCTION_H
#define LISP_BUILDINS_FUNCTION_H

#include <utility>

#include "../parser/Lval.h"
#include "../Constants.h"

namespace buildins {
    enum funcType{
        Global,Local
    };
    parser::Lval *builtin_var(parser::ValueHolder *env, parser::Lval *a,funcType type ) {
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'def' passed incorrect type!");

        /* First argument is symbol list */
        parser::Lval *syms = a->cell[0];

        /* Ensure all elements of first list are symbols */
        for (int i = 0; i < syms->count; i++) {
            LASSERT(a, syms->cell[i]->type == parser::LVAL_SYM,
                    "Function 'def' cannot define non-symbol");
        }

        /* Check correct number of symbols and values */
        LASSERT(a, syms->count == a->count - 1,
                "Function 'def' cannot define incorrect "
                "number of values to symbols");

        /* Assign copies of values to symbols */
        for (int i = 0; i < syms->count; i++) {
            if (type==Global) {
                env->putGlobally(syms->cell[i]->sym, a->cell[i + 1]);
            }

            if (type==Local) {
                env->put(syms->cell[i]->sym, a->cell[i + 1]);
            }
        }

        delete (a);
        return parser::Lval::Lval_sexpr();
    }

    parser::Lval* builtin_def(parser::ValueHolder *env, parser::Lval* a) {
        return builtin_var(env, a, Global);
    }

    parser::Lval* builtin_put(parser::ValueHolder *env, parser::Lval* a) {
        return builtin_var(env, a, Local);
    }

    parser::Lval* builtin_lambda(parser::ValueHolder* env, parser::Lval* a) {
        //TODO Implement checking
        /*LASSERT_NUM("\\", a, 2);
        LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
        LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

        for (int i = 0; i < a->cell[0]->count; i++) {
            LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                    "Cannot define non-symbol. Got %s, Expected %s.",
                    ltype_name(a->cell[0]->cell[i]->type),ltype_name(LVAL_SYM));
        }
*/
        /* Pop first two arguments and pass them to lval_lambda */
        parser::Lval* formals =a->lval_pop();
        parser::Lval* body = a->lval_pop();
        delete a;

        return parser::Lval::Lval_Lambda(formals, body);
    }
}
#endif //LISP_BUILDINS_FUNCTION_H
