//
// Created by eric on 13.12.22.
//

#ifndef LISP_BUILDINS_CONDITINALS_H
#define LISP_BUILDINS_CONDITINALS_H

#include <utility>

#include "../Constants.h"

namespace buildins {
    parser::Lval *builtin_ord(parser::ValueHolder *e, parser::Lval *a, const std::string &op) {
        int r;
        if (op.compare(">") == 0) {
            r = (a->cell[0]->num > a->cell[1]->num);
        }
        if (op.compare("<") == 0) {
            r = (a->cell[0]->num < a->cell[1]->num);
        }
        if (op.compare(">=") == 0) {
            r = (a->cell[0]->num >= a->cell[1]->num);
        }
        if (op.compare("<=") == 0) {
            r = (a->cell[0]->num <= a->cell[1]->num);
        }
        delete a;
        return parser::Lval::Numerical(r);
    }

    parser::Lval *builtin_cmp(parser::ValueHolder *e, parser::Lval *a, const std::string &op) {
        bool r;
        if (op.compare("==") == 0) {
            r = a->cell[0]->equal(a->cell[1]);
        }
        if (op.compare("!=") == 0) {
            r = !a->cell[0]->equal(a->cell[1]);
        }
        delete (a);
        return parser::Lval::Boolean(r);
    }

    parser::Lval *builtin_eq(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_cmp(e, a, "==");
    }

    parser::Lval *builtin_ne(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_cmp(e, a, "!=");
    }

    parser::Lval *builtin_gt(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_ord(e, a, ">");
    }

    parser::Lval *builtin_lt(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_ord(e, a, "<");
    }

    parser::Lval *builtin_ge(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_ord(e, a, ">=");
    }

    parser::Lval *builtin_le(parser::ValueHolder *e, parser::Lval *a) {
        return builtin_ord(e, a, "<=");
    }

    parser::Lval *builtin_if(parser::ValueHolder *e, parser::Lval *a) {
        /* Mark Both Expressions as evaluable */
        parser::Lval *x;
        a->cell[1]->type = parser::LVAL_SEXPR;
        a->cell[2]->type = parser::LVAL_SEXPR;

        if (a->cell[0]->num) {
            /* If condition is true evaluate first expression */
            x = a->pop(1)->eval(e);
        } else {
            /* Otherwise evaluate second expression */
            x = a->pop(2)->eval(e);
        }

        /* Delete argument list and return */
        delete (a);
        return x;
    }
}

#endif //LISP_BUILDINS_CONDITINALS_H
