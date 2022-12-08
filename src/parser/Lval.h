//
// Created by eric on 06.12.22.
//

#ifndef LISP_LVAL_H
#define LISP_LVAL_H

#include <string>
#include <vector>

namespace parser {
    enum LvalTypes {
       LVAL_NONE, LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR
    };

    class Lval {

    public:
        LvalTypes type=LVAL_NONE;
        long num{};
        std::string *err{};
        std::string sym{};
        int count{};
        //TODO: Replace with Queue
        std::vector<Lval *> cell;

        ~Lval();

        Lval* eval();
        Lval* eval_sexpr();

        void lval_add(Lval *x);
        Lval *lval_pop(int i);
        Lval *lval_join(Lval *x, Lval *y);
        Lval *lval_take( int i);
        void lval_expr_print(char open, char close);
        void lval_print();
        void lval_println();


        static void lval_println(Lval *v);
        static Lval* Lval_num(long x) {
            Lval* v = new Lval();
            v->type = LVAL_NUM;
            v->num = x;
            return v;
        }

        static Lval *Lval_Error(std::string m) {
            Lval *v = new Lval();
            v->type = LVAL_ERR;
            v->err = &m;
            return v;
        }

        static Lval *Lval_symbol(std::string s) {
            Lval *v = new Lval();
            v->type = LVAL_SYM;
            v->sym = std::string(s);
            return v;
        }

        static Lval *Lval_sexpr() {
            Lval *v = new Lval();
            v->type = LVAL_SEXPR;
            v->count = 0;
            v->cell.clear();
            return v;
        }

        static Lval *Lval_qexpr() {
            Lval *v = new Lval();
            v->type = LVAL_QEXPR;
            v->count = 0;
            v->cell.clear();
            return v;
        }

    };
} // Lval

#endif //LISP_LVAL_H
