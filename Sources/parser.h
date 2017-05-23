#ifndef ESCHELLE_PARSER_H
#define ESCHELLE_PARSER_H

#include "common.h"
#include "token.h"
#include "ast.h"

namespace Eschelle{
    class Parser{
    private:
        FILE* file_;
        Token* peek_;
        LocalScope* scope_;
        CodeUnit* code_;
        bool private_;

        inline char Peek(){
            int c = getc(file_);
            ungetc(c, file_);
            return (char) c;
        }

        inline char Next(){
            int c = getc(file_);
            return (char) c;
        }

        inline char NextReal(){
            char next;
            while(isspace(next = Next()));
            return next;
        }

        inline bool IsDefinition(TokenKind kind){
            switch(kind){
                case kCLASS:
                case kENUM: return true;
                default: return false;
            }
        }

        inline bool IsSymbol(char c){
            switch(c){
                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                case '=':
                case ':':
                case ';':
                case ',':
                case '.':
                case '/':
                case '+':
                case '*':
                case '-':
                case '_':
                case '?':
                case '<':
                case '>': return true;
                default: return false;
            }
        }

        inline int GetKeyword(std::string txt){
#define LEX(Tk, Name) \
            if(txt == Name) return Tk;
    FOR_EACH_TOKEN(LEX)
#undef LEX
            return -1;
        }

        inline bool IsKeyword(std::string txt){
            return GetKeyword(txt) != -1;
        }

        void ParseFields(Class* cls);
        void ParseParameters(Function* func);
        AstNode* ParseStatement(Class* cls, Function* func);
        AstNode* ParseBinaryExpr();
        AstNode* ParseUnaryExpr();
        Class* ParseDefinition();

        Token* NextToken();
    public:
        Parser():
                file_(nullptr),
                peek_(nullptr),
                scope_(nullptr){}
        ~Parser(){}

        CodeUnit* Parse(std::string file);
    };
}

#endif //ESCHELLE_PARSER_H