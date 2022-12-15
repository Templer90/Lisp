//
// Created by eric on 14.12.22.
//

#ifndef LISP_EVALUATION_H
#define LISP_EVALUATION_H

#include "../parser/Lval.h"
#include "../../includes/mpc/mpc.h"

namespace evaluation{

    parser::Lval *lval_read_num(mpc_ast_t *t) {
        errno = 0;
        long x = strtol(t->contents, nullptr, 10);
        return errno != ERANGE ? parser::Lval::Numerical(x) : parser::Lval::Lval_Error("invalid number");
    }

    parser::Lval *lval_read_str(mpc_ast_t* t) {
        t->contents[strlen(t->contents)-1] = '\0';

        char* unescaped = static_cast<char *>(malloc(strlen(t->contents + 1) + 1));
        strcpy(unescaped, t->contents+1);

        unescaped = static_cast<char *>(mpcf_unescape(unescaped));

        parser::Lval* str = parser::Lval::String(unescaped);

        free(unescaped);
        return str;
    }

    parser::Lval *Lval_Read(mpc_ast_t *t) {
        if (strstr(t->tag, "number")) { return lval_read_num(t); }
        if (strstr(t->tag, "string")) { return lval_read_str(t); }
        if (strstr(t->tag, "symbol")) { return parser::Lval::Symbol(t->contents); }


        parser::Lval *x = nullptr;
        if (strcmp(t->tag, ">") == 0) { x = parser::Lval::S_Expression(); }
        if (strstr(t->tag, "sexpr")) { x = parser::Lval::S_Expression(); }
        if (strstr(t->tag, "qexpr")) { x = parser::Lval::Q_Expression(); }

        for (int i = 0; i < t->children_num; i++) {
            if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
            if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
            if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
            if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
            if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
            if (strstr(t->children[i]->tag, "comment")) { continue; }
            x->add(Lval_Read(t->children[i]));
        }

        return x;
    }
}

#endif //LISP_EVALUATION_H
