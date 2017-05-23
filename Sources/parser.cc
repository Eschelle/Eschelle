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
#define EXPECT(Kind) ({ \
    if((next = NextToken())->GetKind() != Kind){ \
        std::cerr << "Unexpected: " << next->GetText() << "$" << next->GetKind() << std::endl; \
    } \
})

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
            case '{': return new Token(kLBRACE, "{");
            case '}': return new Token(kRBRACE, "}");
            case ';': return new Token(kSEMICOLON, ";");
            case ':': return new Token(kCOLON, ":");
            case '(': return new Token(kLPAREN, "(");
            case ')': return new Token(kRPAREN, ")");
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

    AstNode* Parser::ParseStatement(Class* cls, Function* func){
        Token* next;
        switch((CONSUME)->GetKind()){
            case kSEMICOLON: return nullptr;
            case kIDENTIFIER:{
                std::string ident = next->GetText();
                EXPECT(kSEMICOLON);
                return new StoreStaticFieldNode(cls->GetField(ident), new LiteralNode());
            }
            default:{
                UNEXPECTED;
            }
        }
    }

    void Parser::ParseFields(Class *cls){
        Token* next;
        do{
            EXPECT(kIDENTIFIER);
            std::string ident = next->GetText();
            EXPECT(kCOLON);
            EXPECT(kIDENTIFIER);
            std::string type = next->GetText();

            Class* type_cls = code_->FindClass(type);
            Field* field = cls->DefineField(ident, type_cls, private_);

            switch((CONSUME)->GetKind()){
                case kCOMMA:{
                    continue;
                }
                case kSEMICOLON:{
                    return;
                }
                case kEQUALS:{
                    //TODO: Handle Case?
                    break;
                }
                default: UNEXPECTED;
            }

            if(field != nullptr) std::cout << "Declared field: " << field->GetName() << std::endl;
        } while(true);
    }

    void Parser::ParseParameters(Function *func){
        Token* next;
        while((CONSUME)->GetKind() != kRPAREN){
            switch(next->GetKind()){
                default: UNEXPECTED;
            }
        }
    }

    Class* Parser::ParseDefinition(){
        Class* result = nullptr;

        Token* next;
        do{
            switch((CONSUME)->GetKind()){
                case kPRIVATE:{
                    if(private_){
                        std::cerr << "Already private" << std::endl;
                        getchar();
                        abort();
                    }
                    private_ = true;
                    break;
                }
                case kCLASS:{
                    EXPECT(kIDENTIFIER);
                    std::string identifier = next->GetText();

                    Class* parent = Class::OBJECT;
                    do{
                        switch((CONSUME)->GetKind()){
                            case kEXTENDS:{
                                EXPECT(kIDENTIFIER);
                                parent = code_->FindClass(next->GetText());
                                continue;
                            }
                            case kLBRACE:{
                                result = new Class(identifier, (private_ ? kPrivate : kNone), parent);
                                private_ = false;
                                break;
                            }
                            default: UNEXPECTED;
                        }
                    } while(result == nullptr);

                    do{
                        switch((CONSUME)->GetKind()){
                            case kRBRACE:{
                                return result;
                            }
                            case kPRIVATE:{
                                if(private_){
                                    std::cerr << "Already private" << std::endl;
                                    getchar();
                                    abort();
                                }
                                private_ = true;
                                break;
                            }
                            case kVAR:{
                                ParseFields(result);
                                break;
                            }
                            case kFUNC:{
                                EXPECT(kIDENTIFIER);
                                std::string ident = next->GetText();

                                Class* ret_type = Class::VOID;
                                Function* func = result->DefineFunction(ident, ret_type, private_);

                                EXPECT(kLPAREN);
                                ParseParameters(func);
                                EXPECT(kLBRACE);
                                while((CONSUME)->GetKind() != kRBRACE){
                                    AstNode* stmt = ParseStatement(result, func);
                                    if(stmt != nullptr){
                                        func->AddAst(stmt);
                                    }
                                }
                                break;
                            }
                            default: UNEXPECTED;
                        }
                    } while(true);
                }
                case kENUM:{
                    EXPECT(kIDENTIFIER);
                    std::string identifier = next->GetText();

                    int mods = kFinal;
                    if(private_) mods |= kPrivate;

                    bool defs_done = false;

                    result = new Class(identifier, mods, Class::OBJECT);
                    private_ = false;
                    EXPECT(kLBRACE);
                    while((CONSUME)->GetKind() != kRBRACE){
                        switch(next->GetKind()){
                            case kIDENTIFIER:{
                                if(defs_done){
                                    std::cerr << "Cannot define any more constants" << std::endl;
                                    getchar();
                                    abort();
                                }

                                std::string name = next->GetText();
                                result->DefineStaticField(name, result, false);
                                break;
                            }
                            case kCOMMA:{
                                break;
                            }
                            case kSEMICOLON:{
                                defs_done = true;
                                break;
                            }
                            default: UNEXPECTED;
                        }
                    }
                    return result;
                }
                case kEOF: return result;
                default: UNEXPECTED;
            }
        } while(result != nullptr);
    }

    CodeUnit* Parser::Parse(std::string file){
        file_ = fopen(file.c_str(), "r");
        code_ = new CodeUnit(file);

        while(Peek() != EOF){
            Class* def = ParseDefinition();
            if(def != nullptr){
                code_->AddClass(def);
            }
        }

        fclose(file_);
        return code_;
    }
}