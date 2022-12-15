//
// Created by eric on 10.12.22.
//

#include "ValueHolder.h"

#include <utility>

namespace parser {
    parser::Lval *ValueHolder::get(parser::Lval *k) {
        if (this->map.contains(k->sym)) {
            return this->map[k->sym]->copy();
        } else {
            if (this->parent != nullptr) {
                return parent->get(k);
            } else {
                return parser::Lval::Lval_Error("Unbound symbol '" + k->sym + "'!");
            }
        }
    }

    void ValueHolder::put(parser::Lval *v) {
        if (this->map.contains(v->sym)) {
            auto old = this->map[v->sym];
            this->map.erase(v->sym);
            delete old;
        }

        this->map[v->sym] = v->copy();
    }

    void ValueHolder::put(const std::string &symbol, parser::Lval *v) {
        this->map[symbol] = v->copy();
    }

    void ValueHolder::putGlobally(parser::Lval *v) {
        auto env = this;
        while (env->parent != nullptr) { env = env->parent; }
        env->put(v->sym, v);
    }

    void ValueHolder::putGlobally(const std::string &symbol, parser::Lval *v) {
        auto env = this;
        while (env->parent != nullptr) { env = env->parent; }
        env->put(symbol, v);
    }


    ValueHolder *ValueHolder::copy() {
        auto copy = new ValueHolder();
        copy->Lispy=this->Lispy;
        if (this->parent != nullptr) {
            copy->parent = this->parent;
        } else {
            copy->parent = nullptr;
        }

        for (const auto &[key, value]: this->map) {
            copy->put(key, value);
        }

        return copy;
    }

    ValueHolder::~ValueHolder() = default;

} // parser