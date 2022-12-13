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
            case LVAL_STR:
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

    void Lval::add(Lval *x) {
        this->count++;
        this->cell.push_back(x);
    }

    Lval *Lval::pop() {
        return this->pop(0);
    }

    Lval *Lval::take() {
        return this->take(0);
    }

    Lval *Lval::pop(int i) {
        Lval *x = this->cell[i];
        this->cell.erase(this->cell.begin() + i);
        this->count--;
        return x;
    }

    Lval *Lval::take(int i) {
        Lval *x = this->pop(i);
        for (int j = 0; j < this->count; j++) {
            delete (this->cell[j]);
        }
        this->cell.clear();
        this->count = 0;
        return x;
    }

    Lval *Lval::join(Lval *x, Lval *y) {
        while (y->count) {
            x->add(y->pop());
        }

        delete (y);
        return x;
    }

    std::string Lval::expr_print(char open, char close) {
        std::string ret(1, open);
        for (int i = 0; i < this->count; i++) {
            ret += this->cell[i]->print();
            if (i != (this->count - 1)) {
                ret += ' ';
            }
        }
        ret += close;
        return ret;
    }

    std::string Lval::print() {
        switch (this->type) {
            case LVAL_NUM:
                return std::to_string(this->num);
            case LVAL_ERR:
                return "Error: " + this->err;
            case LVAL_SYM:
                return this->sym;
            case LVAL_SEXPR:
                return this->expr_print('(', ')');
            case LVAL_QEXPR:
                return this->expr_print('{', '}');
            case LVAL_FUN:
                if (this->fun != nullptr) {
                    return "<builtin>";
                } else {
                    return "(\\ " + this->formals->print() + " " + this->body->print() + ")";
                }
            default:
                return "Error in Printing?";
            case LVAL_NONE:
                return "NONE";
            case LVAL_STR:
                return "'"+this->str+"'";
        }
    }

    void Lval::println(Lval *v) {
        auto s = v->print();
        puts(s.c_str());
        putchar('\n');
    }

    void Lval::println() {
        Lval::println(this);
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
            if (list[i]->type == LVAL_ERR) { return this->take(i); }
            if (list[i]->type == LVAL_NONE) { return Lval_Error("NONE Type Generated"); }
        }

        this->cell = list;
        if (this->count == 0) { return this; }
        if (this->count == 1) { return this->take(); }

        Lval *f = this->pop();
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
            case LVAL_STR:
                copy->str= std::string(this->str);
                break;
        }
        return copy;
    }

    Lval *Lval::call(parser::ValueHolder *e, Lval *a) {
        /* If Builtin then simply apply that */
        if (this->fun != nullptr) { return this->fun(e, a); }

        /* Record Argument Counts */
        int given = a->count;
        if (this->formals == nullptr) {
            delete (a);
            return parser::Lval::Lval_Error("No Formals found ");
        }
        int total = this->formals->count;


        /* While arguments still remain to be processed */
        while (a->count) {

            /* If we've ran out of formal arguments to bind */
            if (this->formals->count == 0) {
                delete (a);
                return parser::Lval::Lval_Error("Function passed too many arguments. ");
            }

            /* Pop the first symbol from the formals */
            Lval *symbol = this->formals->pop();

            /* Pop the next argument from the list */
            Lval *val = a->pop();

            /* Bind a copy into the function's environment */
            this->env->put(symbol->sym, val);

            /* Delete symbol and value */
            delete (symbol);
            delete (val);
        }

        /* Argument list is now bound so can be cleaned up */
        delete (a);

        /* If all formals have been bound evaluate */
        if (this->formals->count == 0) {

            /* Set environment parent to evaluation environment */
            this->env->parent = e;

            Lval *newexp = parser::Lval::S_Expression();
            newexp->add(this->body->copy());
            /* Evaluate and return */
            return buildins::builtin_eval(this->env, newexp);
        } else {
            /* Otherwise return partially evaluated function */
            return this->copy();
        }
    }

    Lval *Lval::Numerical(long x) {
        Lval *v = new Lval(LVAL_NUM);
        v->num = x;
        return v;
    }

    Lval *Lval::Lval_Error(std::string m) {
        Lval *v = new Lval(LVAL_ERR);
        v->err = std::move(m);
        return v;
    }

    Lval *Lval::Symbol(std::string s) {
        Lval *v = new Lval(LVAL_SYM);
        v->sym = std::string(std::move(s));
        return v;
    }

    Lval *Lval::S_Expression() {
        Lval *v = new Lval(LVAL_SEXPR);
        return v;
    }

    Lval *Lval::Q_Expression() {
        Lval *v = new Lval(LVAL_QEXPR);
        return v;
    }

    Lval *Lval::Buildin(std::string m, lbuiltin func) {
        Lval *v = new Lval(LVAL_FUN);
        v->sym = std::move(m);
        v->fun = func;
        return v;
    }

    Lval *Lval::Lambda(Lval *formals, Lval *body) {
        Lval *v = new Lval(LVAL_FUN);
        v->fun = nullptr;
        v->env = new ValueHolder();
        v->formals = formals->copy();
        v->body = body->copy();
        return v;
    }

    //Not Clever but easy to read
    Lval *Lval::Boolean(bool x) {
        if (x == true) {
            return Numerical(1);
        } else {
            return Numerical(0);
        }
    }

    Lval *Lval::String(std::string x) {
        Lval *v = new Lval(LVAL_STR);
        v->str = std::move(x);
        return v;
    }

    bool Lval::equal(Lval *other) {
        /* Different Types are always unequal */
        if (this->type != other->type) { return false; }

        /* Compare Based upon type */
        switch (this->type) {
            /* Compare Number Value */
            case LVAL_NUM:
                return (this->num == other->num);

                /* Compare String Values */
            case LVAL_ERR:
                return this->err.compare(other->err) == 0;
            case LVAL_SYM:
                return this->sym.compare(other->sym) == 0;

                /* If builtin compare, otherwise compare formals and body */
            case LVAL_FUN:
                if (this->fun != nullptr || other->fun != nullptr) {
                    return this->fun == other->fun;
                } else {
                    return this->formals->equal(other->formals)
                           && this->body->equal(other->body);
                }

                /* If list compare every individual element */
            case LVAL_QEXPR:
            case LVAL_SEXPR:
                if (this->count != other->count) { return false; }
                for (int i = 0; i < this->count; i++) {
                    /* If any element not equal then whole list not equal */
                    if (!this->cell[i]->equal(other->cell[i])) { return false; }
                }
                /* Otherwise lists must be equal */
                return true;
                break;
            case LVAL_STR:
                return this->str.compare(other->str);
                break;
            case LVAL_NONE:
                return true;
                break;
        }

        return false;
    }
} // Lval