#include <fstream>
#include <sstream>
#include "crossline.h"
#include "src/environment/ValueHolder.h"
#include "includes/mpc/mpc.h"
#include "src/parser/Lval.h"
#include "src/buildins/LvalBuildins.h"
#include "src/evaluation/evaluation.h"


static void completion_hook(char const *buf, crossline_completions_t *pCompletion) {
    int i;
    static const char *cmd[] = {"list", "head", "tail", "eval", "join", "load", "error", "print",
                                "def",
                                "exit", "history", nullptr};

    for (i = 0; nullptr != cmd[i]; ++i) {
        if (0 == strncmp(buf, cmd[i], strlen(buf))) {
            crossline_completion_add(pCompletion, cmd[i], nullptr);
        }
    }
}

void readExecuteLine(std::string line, mpc_parser_t *Lispy, parser::ValueHolder *env){
    mpc_result_t r;
    if (mpc_parse("<stdin>", line.c_str(), Lispy, &r)) {
        parser::Lval *result = evaluation::Lval_Read((mpc_ast_t *) r.output)->eval(env);
        auto actual = result->print();

        puts(result->print().c_str());

        mpc_ast_delete((mpc_ast_t *) r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }
}

void testing(const std::string &file, mpc_parser_t *Lispy, parser::ValueHolder *env) {
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
            parser::Lval *result = evaluation::Lval_Read((mpc_ast_t *) r.output)->eval(env);
            auto actual= result->print();
            if(expectedResult!= result->print()){
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

    puts("testing finished\n");
};

int main() {
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Symbol = mpc_new("symbol");
    mpc_parser_t *String = mpc_new("string");
    mpc_parser_t *Comment = mpc_new("comment");
    mpc_parser_t *Sexpr = mpc_new("sexpr");
    mpc_parser_t *Qexpr = mpc_new("qexpr");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
              "number : /-?[0-9]+/ ;                                                     "
              "symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;                               "
              "string : /\"(\\\\.|[^\"])*\"/ ;                                           "
              "comment: /;[^\\r\\n]*/ ;                                                  "
              "sexpr  : '(' <expr>* ')' ;                                                "
              "qexpr  : '{' <expr>* '}' ;                                                "
              "expr   : <number> | <symbol> | <sexpr> | <qexpr> | <string> | <comment>;  "
              "lispy  : /^/ <expr>* /$/ ;                                                ",
              Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);

    parser::ValueHolder env;
    env.Lispy = Lispy;
    buildins::InitializeBuildins(&env, Lispy);

    puts("Lispy Version 0.9.0.0.2");
    puts("Press Ctrl+c to Exit\n");

    char buf[2048];

    crossline_completion_register(completion_hook);
    crossline_history_load("history.txt");

    testing("./tests/full.test", Lispy, &env);

    puts("Load Standard\n");
    readExecuteLine("load \"standard.lspy\"",Lispy, &env);

    while (nullptr != crossline_readline("Lisp> ", buf, sizeof(buf))) {

        /* Attempt to parse the user input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", buf, Lispy, &r)) {
            parser::Lval *result = evaluation::Lval_Read((mpc_ast_t *) r.output)->eval(&env);
            result->println();
            mpc_ast_delete((mpc_ast_t *) r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
    }

    mpc_cleanup(8, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);

    crossline_history_save("history.txt");
    return 0;
}