//
// Created by eric on 10.12.22.
//

#ifndef LISP_VALUEHOLDER_H
#define LISP_VALUEHOLDER_H

#include <map>
#include <string>
#include "../parser/Lval.h"

namespace parser {
    class Lval;
    class ValueHolder {
    private:
        std::map<std::string, parser::Lval* > map{};
    public:
        ~ValueHolder();
        parser::Lval* get(parser::Lval *k);
        void put (parser::Lval *v);
        void put (const std::string& symbol,parser::Lval *v);
    };

} // parser

#endif //LISP_VALUEHOLDER_H
