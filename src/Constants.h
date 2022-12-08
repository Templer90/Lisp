//
// Created by eric on 06.12.22.
//

#ifndef LISP_CONSTANTS_H
#define LISP_CONSTANTS_H

#include "./parser/Lval.h"

#define LASSERT(args, cond, err) \
  if (!(cond)) { delete(args); return parser::Lval::Lval_Error(err); }

#endif //LISP_CONSTANTS_H
