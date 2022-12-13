//
// Created by eric on 12.12.22.
//

#ifndef LISP_BUILDINS_MATH_H
#define LISP_BUILDINS_MATH_H

#include <utility>

#include "../parser/Lval.h"
#include "../Constants.h"

namespace buildins {
    parser::Lval *builtin_op(parser::ValueHolder *env, parser::Lval *a, std::string *op) {

        for (int i = 0; i < a->count; i++) {
            if (a->cell[i]->type != parser::LVAL_NUM) {
                delete a;
                return parser::Lval::Lval_Error("Cannot operate on non-number!");
            }
        }

        parser::Lval *x = a->pop(0);
        if ((op->compare("-") == 0) && (a->count == 0)) { x->num = -x->num; }

        while (a->count > 0) {

            parser::Lval *y = a->pop(0);

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
}
#endif //LISP_BUILDINS_MATH_H
