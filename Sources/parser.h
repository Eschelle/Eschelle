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
        Class* class_;
        bool private_;

        struct LocalDesc{
            std::string name;
            Class* type;
            AstNode* value;

            LocalDesc(std::string n, Class* t, AstNode* v):
                    name(n),
                    type(t),
                    value(v){}
        };

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

        inline bool IsExpr(TokenKind kind){
            switch(kind){
                case kPLUS:
                case kMINUS:
                case kDIVIDE:
                case kMULTIPLY: return true;
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

        inline int IsDigit(char c){
            return (c >= '0') &&
                    (c <= '9');
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

        void ParseParameters(Function* func);
        void ParseStatement(Function* func);
        void ParseFields();
        void ParseLocals(Function* func);
        AstNode* ParseBinaryExpr();
        AstNode* ParseUnaryExpr();
        Class* ParseDefinition();

        Token* NextToken();
        Token* PeekToken();
    public:
        Parser():
                class_(nullptr),
                code_(nullptr),
                file_(nullptr),
                peek_(nullptr),
                private_(false),
                scope_(nullptr){}
        ~Parser(){}

        CodeUnit* Parse(std::string file);
    };
}

#endif //ESCHELLE_PARSER_H