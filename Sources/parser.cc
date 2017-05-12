#include <cstdio>
#include <cstdlib>
#include "parser.h"

namespace Eschelle{
#define CONSUME \
    next = NextToken()
#define UNEXPECTED ({ \
    std::cerr << "Unexpected: " << next->GetText() << "$" << next->GetKind() << std::endl; \
    getchar(); \
    abort(); \
})
#define DECONSUME \
    peek_ = next;

    AstNode* Parser::ParseUnaryExpr(){
        AstNode* primary = nullptr;

        Token* next;
        switch((CONSUME)->GetKind()){
            default:{
                UNEXPECTED;
            }
        }

        return primary;
    }

    static inline bool IsExpr(Token* tk){
        switch(tk->GetKind()){
            case kMINUS:
            case kPLUS: return true;
            default: return false;
        }
    }

    AstNode* Parser::ParseBinaryExpr(){
        AstNode* left = ParseUnaryExpr();

        Token* next;
        while(IsExpr(CONSUME)){
            // left = new BinaryOpNode(left, ParseBinaryExpr(), next->GetKind());
        }

        DECONSUME
        return left;
    }

    Token* Parser::NextToken(){
        Token* result = nullptr;
        if(peek_ != nullptr){
            result = peek_;
            peek_ = nullptr;
            return result;
        }

        char next = NextReal();
        switch(next){
            case EOF: return new Token(kEOF, "");
            case ',': return new Token(kCOMMA, ",");
            case '=': return new Token(kEQUALS, "=");
            case '+': return new Token(kPLUS, "+");
            case '-': return new Token(kMINUS, "-");
            default: break;
        }

        if(next == '"'){
            std::stringstream stream;
            while((next = Next()) != '"') stream << next;
            result = new Token(kLIT_STRING, stream.str());
        } else if(isdigit(next)){
            std::stringstream stream;
            stream << next;

            bool dbl = false;

            while(isdigit(next = Peek()) || (next == '.' && !dbl)){
                stream << next;
                if(next == '.') dbl = true;
                Next();
            }

            result = new Token(kLIT_NUMBER, stream.str());
        } else{
            std::stringstream stream;
            stream << next;

            while(!isspace(next = Peek()) && !IsSymbol(next)){
                stream << next;
                Next();

                std::string sym = stream.str();
                if(IsKeyword(sym)){
                    result = new Token(static_cast<TokenKind>(GetKeyword(sym)), sym);
                    break;
                }
            }

            if(result == nullptr) result = new Token(kIDENTIFIER, stream.str());
        }

        return result;
    }

    AstNode* Parser::ParseEnum(){
        Token* next;

        // SequenceNode* cstr = new SequenceNode();

        if((CONSUME)->GetKind() != kIDENTIFIER) UNEXPECTED;
        Class* cls = new Class(next->GetText(), kFinal);
        if((CONSUME)->GetKind() != kLBRACE) UNEXPECTED;
        while((CONSUME)->GetKind() != kRBRACE){
            switch(next->GetKind()){
                case kIDENTIFIER:{
                    Field* f = cls->DefineStaticField(next->GetText(), cls);
                    if((CONSUME)->GetKind() != kCOMMA) UNEXPECTED;
                    break;
                }
                default: UNEXPECTED;
            }
        }

        //return cstr;
        return nullptr;
    }

    AstNode* Parser::ParseStatement(){
        Token* next;
        switch((CONSUME)->GetKind()){
            case kSEMICOLON: return nullptr;
            case kENUM:{
                return ParseEnum();
            }
            default:{
                UNEXPECTED;
            }
        }
    }

    AstNode* Parser::Parse(std::string file){
        file_ = fopen(file.c_str(), "r");

        SequenceNode* code = new SequenceNode();

        while(Peek() != EOF){
            AstNode* stmt = ParseStatement();
            if(stmt != nullptr){
                code->Add(stmt);
            }
        }

        fclose(file_);
        return code;
    }
}