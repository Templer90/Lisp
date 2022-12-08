//
// Created by eric on 06.12.22.
//

#include "Lval.h"
#include "../buildins/buildins.h"

namespace parser {
    Lval::~Lval() {
        switch (this->type) {
            case LVAL_NUM:
                break;
            case LVAL_ERR:
                free(this->err);
                break;
            case LVAL_SYM:
                break;

                /* If Qexpr or Sexpr then delete all elements inside */
            case LVAL_QEXPR:
            case LVAL_SEXPR:
                for (int i = 0; i < this->count; i++) {
                    delete (this->cell[i]);
                }
                this->cell.clear();
                break;
        }
        //implied delete(this);
    }

    void Lval::lval_add(Lval *x) {
        this->count++;
        this->cell.push_back(x);
    }

    Lval *Lval::lval_pop(int i) {
        Lval *x = this->cell[i];
        this->cell.erase(this->cell.begin() + i);
        this->count--;
        return x;
    }

    Lval *Lval::lval_join(Lval *x, Lval *y) {
        while (y->count) {
            x->lval_add(y->lval_pop(0));
        }

        delete (y);
        return x;
    }

    Lval *Lval::lval_take(int i) {
        Lval *x = this->lval_pop(i);
        delete this;
        return x;
    }

    void Lval::lval_expr_print(char open, char close) {
        putchar(open);
        for (int i = 0; i < this->count; i++) {

            this->cell[i]->lval_print();

            if (i != (this->count - 1)) {
                putchar(' ');
            }
        }
        putchar(close);
    }

    void Lval::lval_print() {
        switch (this->type) {
            case LVAL_NUM:
                printf("%li", this->num);
                break;
            case LVAL_ERR:
                printf("Error: %s", this->err->c_str());
                break;
            case LVAL_SYM:
                printf("%s", this->sym.c_str());
                break;
            case LVAL_SEXPR:
                this->lval_expr_print('(', ')');
                break;
            case LVAL_QEXPR:
                this->lval_expr_print('{', '}');
                break;
            default:
                printf("%s", "Error in Printing?");
                break;
        }
    }

    void Lval::lval_println(Lval *v) {
        v->lval_print();
        putchar('\n');
    }

    void Lval::lval_println() {
        Lval::lval_println(this);
    }

    Lval* Lval::eval() {
        if (this->type == LVAL_SEXPR) {
            return this->eval_sexpr();
        } else {
            return this;
        }
    }

    Lval* Lval::eval_sexpr() {
        std::vector<Lval*> list ;
        for (int i = 0; i < this->count; i++) {
            list.push_back( this->cell[i]->eval());
        }

        for (int i = 0; i < list.size(); i++) {
            if (list[i]->type == LVAL_ERR) { return this->lval_take(i); }
        }

        if (this->count == 0) { return this; }

        if (this->count == 1) { return this->lval_take( 0); }

        Lval *f = this->lval_pop( 0);
        if (f->type != LVAL_SYM) {
            delete(f);
            delete(this);
            return parser::Lval::Lval_Error("S-expression Does not start with symbol.");
        }

        /* Call builtin with operator */
        Lval *result = buildins::builtin(this, &f->sym);
        delete(f);
        return result;
    }

} // Lval