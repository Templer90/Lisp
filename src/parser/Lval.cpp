//
// Created by eric on 06.12.22.
//

#include "Lval.h"
#include "../buildins/buildins_Misc.h"

namespace parser {
    class ValueHolder;

    Lval::~Lval() {
        switch (this->type) {
            case LVAL_NONE:
            case LVAL_NUM:
            case LVAL_ERR:
            case LVAL_SYM:
                break;
            case LVAL_FUN:
                if (this->fun != nullptr) {
                    delete this->env;
                    delete this->formals;
                    delete this->body;
                }
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
            case LVAL_FUN:
                if (this->fun != nullptr) {
                    return "<builtin>";
                } else {
                    return "(\\ " + this->formals->lval_print() + " " + this->body->lval_print() + ")";
                }
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

    Lval *Lval::eval(ValueHolder *env) {
        if (this->type == LVAL_SYM) {
            Lval *x = env->get(this);
            delete this;
            return x;
        }

        if (this->type == LVAL_SEXPR) {
            return this->eval_sexpr(env);
        }
        return this;
    }

    Lval *Lval::eval_sexpr(ValueHolder *env) {
        std::vector<Lval *> list;
        list.reserve(this->count);
        for (int i = 0; i < this->count; i++) {
            list.push_back(this->cell[i]->eval(env));
        }

        for (int i = 0; i < list.size(); i++) {
            if (list[i]->type == LVAL_ERR) { return this->lval_take(i); }
            if (list[i]->type == LVAL_NONE) { return Lval_Error("NONE Type Generated"); }
        }

        this->cell = list;
        if (this->count == 0) { return this; }
        if (this->count == 1) { return this->lval_take(); }

        Lval *f = this->lval_pop();
        if (f->type != LVAL_FUN) {
            delete (this);
            delete (f);
            return Lval_Error("first element is not a function");
        }

        Lval *result = f->call(env, this);
        delete (f);
        return result;
    }

    Lval *Lval::copy() {
        Lval *copy = new Lval(this->type);
        switch (this->type) {
            case LVAL_FUN:
                if (this->fun != nullptr) {
                    copy->fun = this->fun;
                } else {
                    copy->fun = nullptr;
                    if (this->env != nullptr) {
                        copy->env = this->env->copy();
                    }
                    if (this->formals != nullptr) {
                        copy->formals = this->formals->copy();
                    }
                    if (this->body != nullptr) {
                        copy->body = this->body->copy();
                    }
                }
                break;
            case LVAL_NUM:
                copy->num = this->num;
                break;
            case LVAL_ERR:
                copy->err = this->err;
                break;
            case LVAL_SYM:
                copy->sym = this->sym;
                break;
            case LVAL_SEXPR:
            case LVAL_QEXPR:
                copy->count = this->count;
                copy->cell.reserve(this->count);
                for (int i = 0; i < copy->count; i++) {
                    copy->cell.push_back(this->cell[i]->copy());
                }
                break;
            case LVAL_NONE:
                break;
        }
        return copy;
    }

    Lval* Lval::call(parser::ValueHolder* e, Lval* a) {
        /* If Builtin then simply apply that */
        if (this->fun != nullptr) { return this->fun(e, a); }

        /* Record Argument Counts */
        int given = a->count;
        if (this->formals == nullptr) {
            delete(a);
            return parser::Lval::Lval_Error("No Formals found ");
        }
        int total = this->formals->count;


        /* While arguments still remain to be processed */
        while (a->count) {

            /* If we've ran out of formal arguments to bind */
            if (this->formals->count == 0) {
                delete(a);
                return parser::Lval::Lval_Error("Function passed too many arguments. ");
            }

            /* Pop the first symbol from the formals */
            Lval* symbol = this->formals->lval_pop();

            /* Pop the next argument from the list */
            Lval* val = a->lval_pop();

            /* Bind a copy into the function's environment */
            this->env->put(symbol->sym, val);

            /* Delete symbol and value */
            delete(symbol);
            delete(val);
        }

        /* Argument list is now bound so can be cleaned up */
        delete(a);

        /* If all formals have been bound evaluate */
        if (this->formals->count == 0) {

            /* Set environment parent to evaluation environment */
            this->env->parent = e;

            Lval* newexp=parser::Lval::Lval_sexpr();
            newexp->lval_add(this->body->copy());
            /* Evaluate and return */
            return buildins::builtin_eval(this->env, newexp);
        } else {
            /* Otherwise return partially evaluated function */
            return this->copy();
        }
    }

    Lval *Lval::Lval_num(long x) {
        Lval *v = new Lval(LVAL_NUM);
        v->num = x;
        return v;
    }

    Lval *Lval::Lval_Error(std::string m) {
        Lval *v = new Lval(LVAL_ERR);
        v->err = std::move(m);
        return v;
    }

    Lval *Lval::Lval_symbol(std::string s) {
        Lval *v = new Lval(LVAL_SYM);
        v->sym = std::string(std::move(s));
        return v;
    }

    Lval *Lval::Lval_sexpr() {
        Lval *v = new Lval(LVAL_SEXPR);
        return v;
    }

    Lval *Lval::Lval_qexpr() {
        Lval *v = new Lval(LVAL_QEXPR);
        return v;
    }

    Lval *Lval::Lval_fun(std::string m, lbuiltin func) {
        Lval *v = new Lval(LVAL_FUN);
        v->sym = std::move(m);
        v->fun = func;
        return v;
    }

    Lval *Lval::Lval_Lambda(Lval *formals, Lval *body) {
        Lval *v = new Lval(LVAL_FUN);
        v->fun = nullptr;
        v->env = new ValueHolder();
        v->formals = formals->copy();
        v->body = body->copy();
        return v;
    }
} // Lval