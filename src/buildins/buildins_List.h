//
// Created by eric on 12.12.22.
//

#ifndef LISP_BUILDINS_LIST_H
#define LISP_BUILDINS_LIST_H

#include <utility>

#include "../parser/Lval.h"
#include "../Constants.h"

namespace buildins {
    parser::Lval *builtin_list(parser::ValueHolder *env, parser::Lval *a) {
        a->type = parser::LVAL_QEXPR;
        return a;
    }

    parser::Lval *builtin_head(parser::ValueHolder *env, parser::Lval *a) {
        LASSERT(a, a->count == 1,
                "Function 'head' passed too many arguments.");
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'head' passed incorrect type.");
        LASSERT(a, a->cell[0]->count != 0,
                "Function 'head' passed {}.");

        parser::Lval *v = a->take(0);
        while (v->count > 1) {
            delete (v->pop(1));
        }

        return v;
    }

    parser::Lval *builtin_tail(parser::ValueHolder *env, parser::Lval *a) {
        LASSERT(a, a->count == 1,
                "Function 'tail' passed too many arguments.");
        LASSERT(a, a->cell[0]->type == parser::LVAL_QEXPR,
                "Function 'tail' passed incorrect type.");
        LASSERT(a, a->cell[0]->count != 0,
                "Function 'tail' passed {}.");

        parser::Lval *v = a->take(0);
        parser::Lval *p = v->pop(0);
        delete (p);
        return v;
    }

    parser::Lval *builtin_join(parser::ValueHolder *env, parser::Lval *a) {
        for (int i = 0; i < a->count; i++) {
            LASSERT(a, a->cell[i]->type == parser::LVAL_QEXPR,
                    "Function 'join' passed incorrect type.");
        }

        parser::Lval *x = a->pop(0);

        while (a->count) {
            x->join(x, a->pop(0));
        }

        delete a;
        return x;
    }
}
#endif //LISP_BUILDINS_LIST_H
