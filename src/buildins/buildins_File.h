//
// Created by eric on 14.12.22.
//

#ifndef LISP_BUILDINS_FILE_H
#define LISP_BUILDINS_FILE_H

#include "../Constants.h"
#include "../../includes/mpc/mpc.h"
#include "../evaluation/evaluation.h"

namespace buildins {
    parser::Lval *builtin_load(parser::ValueHolder *e, parser::Lval *a) {
        /* Parse File given by string name */
        mpc_result_t r;
        if (mpc_parse_contents(a->cell[0]->str.c_str(), e->Lispy, &r)) {

            /* Read contents */
            parser::Lval *expr = evaluation::Lval_Read((mpc_ast_t *)r.output);
            mpc_ast_delete((mpc_ast_t *)r.output);

            /* Evaluate each Expression */
            while (expr->count) {
                parser::Lval *x = expr->pop()->eval(e);
                /* If Evaluation leads to error print it */
                if (x->type == parser::LVAL_ERR) { x->println(); }
              delete x;
            }

            delete(expr);
            delete(a);

            /* Return empty list */
            return parser::Lval::S_Expression();
        } else {
            /* Get Parse Error as String */
            char *err_msg = mpc_err_string(r.error);
            mpc_err_delete(r.error);

            /* Create new error message using it */
            parser::Lval *err = parser::Lval::Lval_Error("Could not load Library "+ std::string(err_msg));
            free(err_msg);
            delete a;

            /* Cleanup and return error */
            return err;
        }
    }

    parser::Lval *builtin_print(parser::ValueHolder *e, parser::Lval *a)
    {
        /* Print each argument followed by a space */
        for (int i = 0; i < a->count; i++) {
            puts(a->cell[i]->print().c_str());
            putchar(' ');
        }

        /* Print a newline and delete arguments */
        putchar('\n');
        delete a;

        return parser::Lval::S_Expression();
    }

    parser::Lval *builtin_error(parser::ValueHolder *e, parser::Lval *a){
        /* Construct Error from first argument */
        parser::Lval* err = parser::Lval::Lval_Error(a->cell[0]->str);

        /* Delete arguments and return */
        delete(a);
        return err;
    }
}
#endif //LISP_BUILDINS_FILE_H
