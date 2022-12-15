//
// Created by eric on 15.12.22.
//

#ifndef LISP_BUILDINS_INTERNALS_H
#define LISP_BUILDINS_INTERNALS_H

#include "../parser/Lval.h"

namespace buildins {
    parser::Lval *builtin_env(parser::ValueHolder *e, parser::Lval *a) {
        parser::Lval *list = parser::Lval::Q_Expression();

        for (const auto &[key, value]: e->map) {
            auto container=parser::Lval::S_Expression();
            container->add(parser::Lval::String(key));
            container->add(parser::Lval::String(value->print()));

            list->add(container);
        }

        delete a;
        return list;
    }
}
#endif //LISP_BUILDINS_INTERNALS_H
