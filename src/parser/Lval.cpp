//
// Created by eric on 06.12.22.
//

#include "Lval.h"
#include "../buildins/buildins.h"

namespace parser {
    Lval::~Lval() {
        switch (this->type) {
            case LVAL_NUM:
            case LVAL_ERR:
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
            case LVAL_NONE:
                break;
        }
        //implied delete(this);
    }

    void Lval::lval_add(Lval *x) {
        this->count++;
        this->cell.push_back(x);
    }

    Lval *Lval::lval_pop() {
        return this->lval_pop(0);
    }
    Lval *Lval::lval_take() {
        return this->lval_take(0);
    }

    Lval *Lval::lval_pop(int i) {
        Lval *x = this->cell[i];
        this->cell.erase(this->cell.begin() + i);
        this->count--;
        return x;
    }

    Lval *Lval::lval_take(int i) {
        Lval *x = this->lval_pop(i);
        for (int j = 0; j < this->count; j++) {
            delete (this->cell[j]);
        }
        this->cell.clear();
        this->count = 0;
        return x;
    }

    Lval *Lval::lval_join(Lval *x, Lval *y) {
        while (y->count) {
            x->lval_add(y->lval_pop());
        }

        delete (y);
        return x;
    }

    std::string Lval::lval_expr_print(char open, char close) {
        std::string str(1, open);
        for (int i = 0; i < this->count; i++) {
            str += this->cell[i]->lval_print();
            if (i != (this->count - 1)) {
                str += ' ';
            }
        }
        str += close;
        return str;
    }

    std::string Lval::lval_print() {
        switch (this->type) {
            case LVAL_NUM:
                return std::to_string(this->num);
            case LVAL_ERR:
                return "Error: " + this->err;
            case LVAL_SYM:
                return this->sym;
            case LVAL_SEXPR:
                return this->lval_expr_print('(', ')');
            case LVAL_QEXPR:
                return this->lval_expr_print('{', '}');
            default:
                return "Error in Printing?";
        }
    }

    void Lval::lval_println(Lval *v) {
        auto s = v->lval_print();
        puts(s.c_str());
        putchar('\n');
    }

    void Lval::lval_println() {
        Lval::lval_println(this);
    }

    Lval *Lval::eval() {
        if (this->type == LVAL_SEXPR) {
            return this->eval_sexpr();
        } else {
            return this;
        }
    }

    Lval *Lval::eval_sexpr() {
        std::vector<Lval *> list;
        for (int i = 0; i < this->count; i++) {
            list.push_back(this->cell[i]->eval());
        }

        for (int i = 0; i < list.size(); i++) {
            if (list[i]->type == LVAL_ERR) { return this->lval_take(i); }
            if (list[i]->type == LVAL_NONE) { return Lval_Error("NONE Type Generated"); }
        }

        this->cell=list;
        if (this->count == 0) { return this; }

        if (this->count == 1) { return this->lval_take(); }

        Lval *f = this->lval_pop();
        if (f->type != LVAL_SYM) {
            delete (f);
            delete (this);
            return parser::Lval::Lval_Error("S-expression Does not start with symbol.");
        }

        /* Call builtin with operator */
        auto symbol=f->sym;
        Lval *result = buildins::builtin(this, &symbol);
        delete (f);
        return result;
    }

} // Lval