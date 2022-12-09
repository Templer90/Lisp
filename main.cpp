#include <fstream>
#include <sstream>
#include "crossline.h"
#include "includes/mpc/mpc.h"
#include "src/parser/Lval.h"

static void completion_hook(char const *buf, crossline_completions_t *pCompletion) {
    int i;
    static const char *cmd[] = {"list", "head", "tail", "eval", "join", "exit", "history", nullptr};

    for (i = 0; nullptr != cmd[i]; ++i) {
        if (0 == strncmp(buf, cmd[i], strlen(buf))) {
            crossline_completion_add(pCompletion, cmd[i], nullptr);
        }
    }
}


parser::Lval *lval_read_num(mpc_ast_t *t) {
    errno = 0;
    long x = strtol(t->contents, nullptr, 10);
    return errno != ERANGE ? parser::Lval::Lval_num(x) : parser::Lval::Lval_Error("invalid number");
}

parser::Lval *lval_read(mpc_ast_t *t) {
    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return parser::Lval::Lval_symbol(t->contents); }

    parser::Lval *x = nullptr;
    if (strcmp(t->tag, ">") == 0) { x = parser::Lval::Lval_sexpr(); }
    if (strstr(t->tag, "sexpr")) { x = parser::Lval::Lval_sexpr(); }
    if (strstr(t->tag, "qexpr")) { x = parser::Lval::Lval_qexpr(); }

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
        x->lval_add(lval_read(t->children[i]));
    }

    return x;
}

void testing( const std::string& file,mpc_parser_t *Lispy){
    puts("testing started\n");

    std::ifstream testfile(file);
    std::string line;
    std::string expectedResult;
    while(getline(testfile, line))
    {
        if(line.empty())continue;
        getline(testfile, expectedResult);
        mpc_result_t r;
        if (mpc_parse("<stdin>", line.c_str(), Lispy, &r)) {
            parser::Lval *result = lval_read((mpc_ast_t *) r.output)->eval();
            auto actual=result->lval_print();
            if(expectedResult!=result->lval_print()){
                mpc_ast_print((mpc_ast_t *)r.output);
                printf("'%s' is different '%s' <> '%s'",line.c_str(),actual.c_str(), expectedResult.c_str() );
                puts("\n");
            }
            mpc_ast_delete((mpc_ast_t *) r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
    }
};

int main() {
    /* Create Some Parsers */
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Symbol = mpc_new("symbol");
    mpc_parser_t *Sexpr = mpc_new("sexpr");
    mpc_parser_t *Qexpr = mpc_new("qexpr");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
              " number : /-?[0-9]+/ ;                              "
              " symbol : \"list\" | \"head\" | \"tail\" | \"eval\" "
              "        | \"join\" | '+' | '-' | '*' | '/' ;        "
              " sexpr  : '(' <expr>* ')' ;                         "
              " qexpr  : '{' <expr>* '}' ;                         "
              " expr   : <number> | <symbol> | <sexpr> | <qexpr> ; "
              " lispy  : /^/ <expr>* /$/ ;                         ",
              Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    puts("Lispy Version 0.0.0.0.2");
    puts("Press Ctrl+c to Exit\n");

    char buf[2048];

    crossline_completion_register(completion_hook);
    crossline_history_load("history.txt");

    testing("./tests/test.test",Lispy);

    while (nullptr != crossline_readline("Lisp> ", buf, sizeof(buf))) {

        /* Attempt to parse the user input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", buf, Lispy, &r)) {
            mpc_ast_print((mpc_ast_t *)r.output);

            parser::Lval *result = lval_read((mpc_ast_t *) r.output)->eval();
            result->lval_println();
            mpc_ast_delete((mpc_ast_t *) r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
    }

    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
    crossline_history_save("history.txt");
    return 0;
}