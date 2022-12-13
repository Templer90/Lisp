//
// Created by eric on 12.12.22.
//

#ifndef LISP_BUILDINS_MISC_H
#define LISP_BUILDINS_MISC_H

#include <utility>

#include "../parser/Lval.h"
#include "../Constants.h"

namespace buildins {
    parser::Lval *builtin_eval(parser::ValueHolder *env, parser::Lval *a);
}

#endif //LISP_BUILDINS_MISC_H
