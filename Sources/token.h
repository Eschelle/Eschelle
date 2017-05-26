#ifndef ESCHELLE_TOKEN_H
#define ESCHELLE_TOKEN_H

#include <string>
#include "common.h"

namespace Eschelle{
#define FOR_EACH_TOKEN_KW(V) \
    V(kENUM, "enum") \
    V(kCLASS, "class") \
    V(kPRIVATE, "private") \
    V(kEXTENDS, "extends") \
    V(kNIL, "nil") \
    V(kVAR, "var") \
    V(kFUNC, "func") \
    V(kPROTO, "proto") \
    V(kOBJECT, "object") \
    V(kRETURN, "return") \
    V(kTRUE, "true") \
    V(kFALSE, "false")

#define FOR_EACH_TOKEN_SYM(V) \
    V(kEQUALS, "=") \
    V(kLBRACE, "{") \
    V(kRBRACE, "}") \
    V(kSEMICOLON, ";") \
    V(kCOMMA, ",") \
    V(kPLUS, "+") \
    V(kMINUS, "-") \
    V(kMULTIPLY, "*") \
    V(kDIVIDE, "/") \
    V(kCOLON, ":") \
    V(kLPAREN, "(") \
    V(kRPAREN, ")")

#define FOR_EACH_TOKEN_LIT(V) \
    V(kLIT_STRING, "<literal string>") \
    V(kLIT_NUMBER, "<literal number>")

#define FOR_EACH_TOKEN(V) \
    FOR_EACH_TOKEN_LIT(V) \
    FOR_EACH_TOKEN_SYM(V) \
    FOR_EACH_TOKEN_KW(V)

    enum TokenKind{
#define DEFINE_TOKEN(Tk, Name) Tk,
        FOR_EACH_TOKEN(DEFINE_TOKEN)
#undef DEFINE_TOKEN
        kIDENTIFIER,
        kEOF
    };

    class Token{
    private:
        std::string text_;
        TokenKind kind_;
    public:
        Token(TokenKind kind, std::string text):
                text_(text),
                kind_(kind){}

        TokenKind GetKind() const{
            return kind_;
        }

        std::string GetText() const{
            return text_;
        }
    };
}

#endif //ESCHELLE_TOKEN_H
