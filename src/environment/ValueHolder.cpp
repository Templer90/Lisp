//
// Created by eric on 10.12.22.
//

#include "ValueHolder.h"

#include <utility>

namespace parser {
    parser::Lval *ValueHolder::get(parser::Lval *k) {
        if (this->map.contains(k->sym)) {
            return this->map[k->sym]->copy();
        }

        return parser::Lval::Lval_Error("unbound symbol!");
    }

    void ValueHolder::put(parser::Lval *v) {
        if (this->map.contains(v->sym)) {
            auto old = this->map[v->sym];
            this->map.erase(v->sym);
            delete old;
        }

        this->map[v->sym] = v->copy();
    }

    void ValueHolder::put(const std::string& symbol, parser::Lval *v) {
       // parser::Lval* a = parser::Lval::Lval_symbol(std::move(symbol));

        this->map[symbol] = v->copy();
    }

    ValueHolder::~ValueHolder() = default;

} // parser