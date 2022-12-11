//
// Created by eric on 06.12.22.
//

#ifndef LISP_BUILDINS_H
#define LISP_BUILDINS_H

#include <utility>

#include "../parser/Lval.h"
#include "../Constants.h"

namespace buildins {
    parser::Lval *builtin_list(parser::ValueHolder *env,parser::Lval *a) {
        a->type = parser::LVAL_QEXPR;
        return a;
    }

    parser::Lval *builtin_head(parser::ValueHolder *env,parser::Lval *a) {
        LASSERT(a, a->count == 1,
                "Function 'head' passed too many arguments.");
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'head' passed incorrect type.");
        LASSERT(a, a->cell[0]->count != 0,
                "Function 'head' passed {}.");

        parser::Lval *v = a->lval_take(0);
        while (v->count > 1) {
            delete (v->lval_pop(1));
        }

        return v;
    }

    parser::Lval *builtin_tail(parser::ValueHolder *env,parser::Lval *a) {
        LASSERT(a, a->count == 1,
                "Function 'tail' passed too many arguments.");
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'tail' passed incorrect type.");
        LASSERT(a, a->cell[0]->count != 0,
                "Function 'tail' passed {}.");

        parser::Lval *v = a->lval_take(0);
        parser::Lval *p= v->lval_pop(0);
        delete (p);
        return v;
    }

    parser::Lval *builtin_eval(parser::ValueHolder *env,parser::Lval *a) {
        LASSERT(a, a->count == 1,
                "Function 'eval' passed too many arguments.");
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'eval' passed incorrect type.");

        parser::Lval *x = a->lval_take(0);
        x->type = parser::LVAL_SEXPR;
        return x->eval(env);
    }

    parser::Lval* builtin_def(parser::ValueHolder *env,parser::Lval *a) {
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'def' passed incorrect type!");

        /* First argument is symbol list */
        parser::Lval* syms = a->cell[0];

        /* Ensure all elements of first list are symbols */
        for (int i = 0; i < syms->count; i++) {
            LASSERT(a, syms->cell[i]->type == parser::LVAL_SYM,
                    "Function 'def' cannot define non-symbol");
        }

        /* Check correct number of symbols and values */
        LASSERT(a, syms->count == a->count-1,
                "Function 'def' cannot define incorrect "
                "number of values to symbols");

        /* Assign copies of values to symbols */
        for (int i = 0; i < syms->count; i++) {
            //syms->cell[i]->cell[0]=a->cell[i+1];
            env->put(syms->cell[i]->sym,a->cell[i+1]);
        }

        delete(a);
        return parser::Lval::Lval_sexpr();
    }

    parser::Lval *builtin_join(parser::ValueHolder *env,parser::Lval *a) {

        for (int i = 0; i < a->count; i++) {
            LASSERT(a, a->cell[i]->type == parser::LVAL_QEXPR,
                    "Function 'join' passed incorrect type.");
        }

        parser::Lval *x = a->lval_pop(0);

        while (a->count) {
            x->lval_join(x, a->lval_pop(0));
        }

        delete a;
        return x;
    }

    parser::Lval *builtin_op(parser::ValueHolder *env, parser::Lval *a, std::string *op) {

        for (int i = 0; i < a->count; i++) {
            if (a->cell[i]->type != parser::LVAL_NUM) {
                delete a;
                return parser::Lval::Lval_Error("Cannot operate on non-number!");
            }
        }

        parser::Lval *x = a->lval_pop(0);
        if ((op->compare("-") == 0) && (a->count == 0)) { x->num = -x->num; }

        while (a->count > 0) {

            parser::Lval *y = a->lval_pop(0);

            if (op->compare("+") == 0) { x->num += y->num; }
            if (op->compare("-") == 0) { x->num -= y->num; }
            if (op->compare("*") == 0) { x->num *= y->num; }
            if (op->compare("/") == 0) {
                if (y->num == 0) {
                   delete x;
                   delete y;
                    x = parser::Lval::Lval_Error("Division By Zero.");
                    break;
                }
                x->num /= y->num;
            }

            delete y;
        }

        delete a;
        return x;
    }

    parser::Lval *builtin_add(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_op(e, a, new std::string("+"));
    }

    parser::Lval *builtin_sub(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_op(e, a, new std::string("-"));
    }

    parser::Lval *builtin_mul(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_op(e, a, new std::string("*"));
    }

    parser::Lval *builtin_div(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_op(e, a, new std::string("/"));
    }

    void lenv_add_builtin(parser::ValueHolder *env, std::string name, parser::lbuiltin func) {
        env->put(parser::Lval::Lval_fun(std::move(name),func));
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
        lenv_add_builtin(env, "def",  builtin_def);
    }
}
#endif //LISP_BUILDINS_H
