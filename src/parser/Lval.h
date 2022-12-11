//
// Created by eric on 06.12.22.
//

#ifndef LISP_LVAL_H
#define LISP_LVAL_H

#include <string>
#include <utility>
#include <vector>
#include <list>
#include <queue>
#include "../environment/ValueHolder.h"

namespace parser {
    class ValueHolder;
    enum LvalTypes {
        LVAL_NONE,LVAL_FUN,LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR
    };

    class Lval;
    typedef Lval*(*lbuiltin)(ValueHolder*, Lval*);

    class Lval {
    private:

    public:
        LvalTypes type = LVAL_NONE;
        long num{};
        std::string err{};
        std::string sym{};
        lbuiltin fun;

        //TODO: Replace with Queue
        int count{};
        std::vector<Lval *> cell;

        explicit Lval(LvalTypes t) {
            this->type = t;
            this->count = 0;
            this->cell.clear();
            this->fun=[](ValueHolder*, Lval*){
                return Lval_Error("first element is not a function");
            };
        }

        ~Lval();

        Lval *eval(ValueHolder *env);

        Lval *eval_sexpr(ValueHolder *env);

        void lval_add(Lval *x);

        Lval *lval_pop(int i);

        Lval *lval_take(int i);

        Lval *lval_pop();

        Lval *lval_take();

        Lval *lval_join(Lval *x, Lval *y);

        std::string lval_expr_print(char open, char close);

        std::string lval_print();

        void lval_println();

        Lval* copy();

        static void lval_println(Lval *v);

        static Lval *Lval_num(long x) {
            Lval *v = new Lval(LVAL_NUM);
            v->num = x;
            return v;
        }

        static Lval *Lval_Error(std::string m) {
            Lval *v = new Lval(LVAL_ERR);
            v->err = std::move(m);
            return v;
        }

        static Lval *Lval_symbol(std::string s) {
            Lval *v = new Lval(LVAL_SYM);
            v->sym = std::string(std::move(s));
            return v;
        }

        static Lval *Lval_sexpr() {
            Lval *v = new Lval(LVAL_SEXPR);
            return v;
        }

        static Lval *Lval_qexpr() {
            Lval *v = new Lval(LVAL_QEXPR);
            return v;
        }

        static Lval *Lval_fun(std::string m, lbuiltin func) {
            Lval *v = new Lval(LVAL_FUN);
            v->sym = std::move(m);
            v->fun = func;
            return v;
        }

    };
} // Lval

#endif //LISP_LVAL_H
