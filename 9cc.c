#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンノシュルイ
typedef enum {
  TK_RESERVED,	//記号
  TK_NUM,	//整数トークン
  TK_EOF,	//入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind Kind;	// トークンの型
  Token *next;		// 次の入力トークン
  int val;		// kindがTK_NUMの場合、その数値
  char *str;		// token string
};

// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}


// 次のトークンが期待している記号のときには、トークンを一つ読み進めて
// 真を返す。それ以外のときは偽を返す。
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}
// 次のトークンがきたいしている記号の時には、トークンを一つ読み進める。
// それ以外の場合にはえらーを　報告する。
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("'%c'ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンをさくせいして　curに繋げる。
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token)));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字列ｐをトークナイズしてそれを返す。
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM,cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
} 

int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
 
  token = tokenize(argv[1]);

  // アセンブリの前半部分を出力 
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  
  // 式の最初は数でなければならないので、それをちぇっくして
  // 最初のmov命令を出力
  printf(" mov rax, %d\n", expect_number());

  while(!at_eof()) {
    if (cosume('+')) {
      printf(" add rax, %d\n", expect_number());
      continue;
    }
   
    expect('-');
    printf(" sub rax, %d\n", expect_number());
  } 

  printf("  ret\n");
  return 0;
}

// input program
char *user_input;

// errorを報告
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int pos = loc - user_input;
  fprintf(stderr, "%s/n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "/n");
  exit(1);
}



