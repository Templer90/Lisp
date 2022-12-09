//
// Created by eric on 06.12.22.
//

#ifndef LISP_BUILDINS_H
#define LISP_BUILDINS_H

#include "../parser/Lval.h"
#include "../Constants.h"

namespace buildins {
    parser::Lval *builtin_list(parser::Lval *a) {
        a->type = parser::LVAL_QEXPR;
        return a;
    }

    parser::Lval *builtin_head(parser::Lval *a) {
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

    parser::Lval *builtin_tail(parser::Lval *a) {
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

    parser::Lval *builtin_eval(parser::Lval *a) {
        LASSERT(a, a->count == 1,
                "Function 'eval' passed too many arguments.");
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'eval' passed incorrect type.");

        parser::Lval *x = a->lval_take(0);
        x->type = parser::LVAL_SEXPR;
        return x->eval();
    }

    parser::Lval *builtin_join(parser::Lval *a) {

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

    parser::Lval *builtin_op(parser::Lval *a, std::string *op) {

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

    parser::Lval *builtin(parser::Lval *a, std::string *func) {
        if (func->compare("list") == 0) { return builtin_list(a); }
        if (func->compare("head") == 0) { return builtin_head(a); }
        if (func->compare("tail") == 0) { return builtin_tail(a); }
        if (func->compare("join") == 0) { return builtin_join(a); }
        if (func->compare("eval") == 0) { return builtin_eval(a); }

        if (func->compare("+") == 0) { return builtin_op(a, func); }
        if (func->compare("-") == 0) { return builtin_op(a, func); }
        if (func->compare("/") == 0) { return builtin_op(a, func); }
        if (func->compare("*") == 0) { return builtin_op(a, func); }
        delete (a);
        return parser::Lval::Lval_Error("Unknown Function!");
    }
}
#endif //LISP_BUILDINS_H
