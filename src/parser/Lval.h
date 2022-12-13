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
        std::string expr_print(char open, char close);
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

        void add(Lval *x);

        Lval *pop(int i);

        Lval *pop();

        Lval *take(int i);

        Lval *take();

        Lval *join(Lval *x, Lval *y);

        bool equal(Lval *other);

        Lval *call(parser::ValueHolder* e, Lval* a);

        std::string print();

        void println();

        Lval *copy();

        static void println(Lval *v);

        static Lval *Numerical(long x);

        static Lval *Boolean(bool x);

        static Lval *Lval_Error(std::string m);

        static Lval *Symbol(std::string s);

        static Lval *S_Expression();

        static Lval *Q_Expression();

        static Lval *Buildin(std::string m, lbuiltin func);

        static Lval *Lambda(parser::Lval *formals, parser::Lval *body);
    };
} // Lval

#endif //LISP_LVAL_H
