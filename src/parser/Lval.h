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
        LVAL_NONE, LVAL_FUN, LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR
    };

    class Lval;

    typedef Lval *(*lbuiltin)(ValueHolder *, Lval *);

    class Lval {
    private:

    public:
        LvalTypes type = LVAL_NONE;

        //Atoms:
        long num{};
        std::string err{};
        std::string sym{};

        //Function:
        lbuiltin fun;
        ValueHolder *env;
        Lval *formals;
        Lval* body;

        //TODO: Replace with Queue
        int count{};
        std::vector<Lval *> cell;

        explicit Lval(LvalTypes t) {
            this->type = t;
            this->count = 0;
            this->cell.clear();
            this->fun = nullptr;
            this->env = nullptr;
            this->formals = nullptr;
            this->body = nullptr;
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

        Lval *call(parser::ValueHolder* e, Lval* a);

        std::string lval_expr_print(char open, char close);

        std::string lval_print();

        void lval_println();

        Lval *copy();

        static void lval_println(Lval *v);

        static Lval *Lval_num(long x);

        static Lval *Lval_Error(std::string m);

        static Lval *Lval_symbol(std::string s);

        static Lval *Lval_sexpr();

        static Lval *Lval_qexpr();

        static Lval *Lval_fun(std::string m, lbuiltin func);

        static Lval *Lval_Lambda(parser::Lval *formals, parser::Lval *body);
    };
} // Lval

#endif //LISP_LVAL_H
