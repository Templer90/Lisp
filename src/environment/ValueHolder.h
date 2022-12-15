//
// Created by eric on 10.12.22.
//

#ifndef LISP_VALUEHOLDER_H
#define LISP_VALUEHOLDER_H

#include <map>
#include <string>
#include "../parser/Lval.h"
#include "../../includes/mpc/mpc.h"

namespace parser {
    class Lval;

    class ValueHolder {
    private:
        std::string name = "Not Set";
    public:
        std::map<std::string, parser::Lval *> map{};
        ValueHolder *parent = nullptr;
        mpc_parser_t *Lispy= nullptr;


        ~ValueHolder();

        parser::Lval *get(parser::Lval *k);

        void put(parser::Lval *v);

        void put(const std::string &symbol, parser::Lval *v);

        void putGlobally(parser::Lval *v);
        void putGlobally(const std::string &symbol, parser::Lval *v);

        ValueHolder *copy();
    };

} // parser

#endif //LISP_VALUEHOLDER_H
