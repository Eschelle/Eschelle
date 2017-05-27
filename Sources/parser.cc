#include <cstdio>
#include <cstdlib>
#include "parser.h"

namespace Eschelle{
#define CONSUME \
    next = NextToken()
#define UNEXPECTED ({ \
    std::cerr << __LINE__ << std::endl; \
    std::cerr << "Unexpected: " << next->GetText() << "$" << next->GetKind() << std::endl; \
    getchar(); \
    abort(); \
})
#define DECONSUME \
    peek_ = next;
#define EXPECT(Kind) ({ \
    if((next = NextToken())->GetKind() != Kind){ \
        std::cerr << __LINE__ << std::endl; \
        std::cerr << "Unexpected: " << next->GetText() << "$" << next->GetKind() << std::endl; \
        getchar(); \
        abort(); \
    } \
})

    AstNode* Parser::ParseUnaryExpr(){
        Token* next;
        switch((CONSUME)->GetKind()){
            case kLIT_NUMBER:{
                std::string txt = next->GetText();
                if(txt.find('.', 0) > 0){
                    return new LiteralNode(new Double(atof(txt.c_str())));
                } else{
                    return new LiteralNode(new Int(atoi(txt.c_str())));
                }
                break;
            }
            case kLIT_STRING:{
                return new LiteralNode(new String(next->GetText()));
            }
            case kTRUE: return new LiteralNode(Bool::TRUE);
            case kFALSE: return new LiteralNode(Bool::FALSE);
            case kIDENTIFIER:{
                std::string name = next->GetText();
                LocalVariable* local = nullptr;
                if(scope_->Lookup(name, &local)){
                    return new LoadLocalNode(local);
                }

                Field* field = nullptr;
                if((field = class_->GetField(name)) != nullptr){
                    return field->CreateLoad();
                }

                std::cerr << "Undeclared identifier '" << name << "'" << std::endl;
                getchar();
                abort();
            }
            default:{
                UNEXPECTED;
            }
        }
    }

    AstNode* Parser::ParseBinaryExpr(){
        AstNode* left = ParseUnaryExpr();

        Token* next;
        while(IsExpr((CONSUME)->GetKind())){
            left = new BinaryOpNode(left, ParseBinaryExpr(), next->GetKind());
        }

        DECONSUME
        return left;
    }

    Token* Parser::PeekToken(){
        peek_ = NextToken();
        return peek_;
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
            case '*': return new Token(kMULTIPLY, "*");
            case '/': return new Token(kDIVIDE, "/");
            default: break;
        }

        if(next == '"'){
            std::stringstream stream;
            while((next = Next()) != '"') stream << next;
            result = new Token(kLIT_STRING, stream.str());
        } else if(IsDigit(next)){
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

    void Parser::ParseStatement(Function* func){
        Token* next;
        switch((CONSUME)->GetKind()){
            case kSEMICOLON: return;
            case kVAR:{
                ParseLocals(func);
                break;
            }
            case kRETURN:{
                func->AddAst(new ReturnNode(ParseBinaryExpr()));
                break;
            }
            case kIDENTIFIER:{
                std::string name = next->GetText();
                EXPECT(kEQUALS);

                LocalVariable* local = nullptr;
                if(scope_->Lookup(name, &local)){
                    func->AddAst(new StoreLocalNode(local, ParseBinaryExpr()));
                    break;
                }

                Field* field = nullptr;
                if((field = class_->GetField(name)) != nullptr){
                    func->AddAst(field->CreateStore(ParseBinaryExpr()));
                    break;
                }

                std::cerr << "Undeclared identifier '" << name << "'" << std::endl;
                getchar();
                abort();
            }
            default:{
                UNEXPECTED;
            }
        }
    }

    void Parser::ParseLocals(Function* func){
        Token* next = nullptr;
        entry:
            EXPECT(kIDENTIFIER);
            std::string ident = next->GetText();
            EXPECT(kCOLON);
            EXPECT(kIDENTIFIER);
            Class* type_cls = code_->FindClass(next->GetText());

            LocalVariable* local = new LocalVariable(type_cls, ident);
            if(!scope_->AddLocal(local)){
                std::cerr << "Cannot add local variable '" << local->GetName() << "'" << std::endl;
                getchar();
                abort();
            }
        exit_0:
            switch((CONSUME)->GetKind()){
                case kCOMMA: goto entry;
                case kSEMICOLON: goto exit_1;
                case kEQUALS:{
                    func->AddAst(new StoreLocalNode(local, ParseBinaryExpr()));
                    goto exit_0;
                }
                default: UNEXPECTED;
            }
        exit_1:
            private_ = false;
    }

    void Parser::ParseFields(){
        Token* next = nullptr;
        entry:
            EXPECT(kIDENTIFIER);
            std::string ident = next->GetText();
            EXPECT(kCOLON);
            EXPECT(kIDENTIFIER);
            Class* type_cls = code_->FindClass(next->GetText());

            Field* field = class_->CreateField(ident, type_cls, private_);
        exit_0:
            switch((CONSUME)->GetKind()){
                case kCOMMA: goto entry;
                case kSEMICOLON: goto exit_1;
                case kEQUALS:{
                    class_->GetConstructor()->AddAst(field->CreateStore(ParseBinaryExpr()));
                    goto exit_0;
                }
                default: UNEXPECTED;
            }
        exit_1:
            private_ = false;
    }

    void Parser::ParseParameters(Function *func){
        Token* next;
        while((CONSUME)->GetKind() != kRPAREN){
            switch(next->GetKind()){
                case kIDENTIFIER:{
                    std::string ident = next->GetText();
                    EXPECT(kCOLON);
                    EXPECT(kIDENTIFIER);
                    std::string type = next->GetText();

                    Class* type_cls = code_->FindClass(type);
                    LocalVariable* param = new LocalVariable(type_cls, ident);

                    if((CONSUME)->GetKind() == kEQUALS){
                        AstNode* expr = ParseBinaryExpr();
                        if(!expr->IsConstantExpr()){
                            UNEXPECTED;
                        }
                        param->SetConstantValue(expr->EvalConstantExpr());
                    } else{
                        DECONSUME;
                    }

                    if(!scope_->AddLocal(param)){
                        std::cerr << "Unable to add parameter: " << ident << std::endl;
                        getchar();
                        abort();
                    }
                    break;
                }
                case kRPAREN: return;
                default: UNEXPECTED;
            }
        }
    }

    Class* Parser::ParseDefinition(){
        scope_ = new LocalScope();


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
                case kOBJECT:{
                    EXPECT(kIDENTIFIER);
                    std::string obj_ident = next->GetText();

                    Class* parent = Class::OBJECT;
                    _o_entry:
                        switch((CONSUME)->GetKind()){
                            case kLBRACE:{
                                class_ = new Class(obj_ident, (private_ ? kPrivate|kFinal : kFinal), parent, kObjectClass);
                                private_ = false;
                                break;
                            }
                            case kEXTENDS:{
                                EXPECT(kIDENTIFIER);
                                parent = code_->FindClass(next->GetText());
                                goto _o_entry;
                            }
                            default: UNEXPECTED;
                        }

                    do{
                        switch((CONSUME)->GetKind()){
                            case kPRIVATE:{
                                private_ = true;
                                break;
                            }
                            case kVAR:{
                                ParseFields();
                                break;
                            }
                            case kIDENTIFIER:{
                                std::string type_cls = next->GetText();
                                EXPECT(kIDENTIFIER);
                                std::string name = next->GetText();

                                Class* ret_type = code_->FindClass(type_cls);
                                Function* func = class_->DefineStaticFunction(name, ret_type, private_);
                                private_ = false;

                                EXPECT(kLPAREN);
                                ParseParameters(func);
                                EXPECT(kLBRACE);
                                do{
                                    ParseStatement(func);
                                } while(PeekToken()->GetKind() != kRBRACE);
                                EXPECT(kRBRACE);
                                break;
                            }
                            case kFUNC:{
                                EXPECT(kIDENTIFIER);
                                std::string ident = next->GetText();

                                Class* ret_type = Class::VOID;
                                Function* func = class_->DefineStaticFunction(ident, ret_type, private_);
                                private_ = false;
                                func->AttachScope(scope_ = new LocalScope(scope_));

                                EXPECT(kLPAREN);
                                ParseParameters(func);
                                EXPECT(kLBRACE);
                                do{
                                    ParseStatement(func);
                                } while(PeekToken()->GetKind() != kRBRACE);
                                EXPECT(kRBRACE);

                                scope_ = scope_->GetParent();
                                break;
                            }
                            case kRBRACE:{
                                return class_;
                            }
                            default: UNEXPECTED;
                        }
                    } while(true);
                }
                case kPROTO:{
                    EXPECT(kIDENTIFIER);
                    std::string cls_ident = next->GetText();

                    Class* parent = Class::OBJECT;
                    _p_entry:
                        switch((CONSUME)->GetKind()){
                            case kLBRACE:{
                                class_ = new Class(cls_ident, (private_ ? kPrivate : kNone), parent, kProtoClass);
                                private_ = false;
                                break;
                            }
                            default: UNEXPECTED;
                        }

                    do{
                        switch((CONSUME)->GetKind()){
                            case kVAR:{
                                ParseFields();
                                break;
                            }
                            case kFUNC:{
                                EXPECT(kIDENTIFIER);
                                std::string ident = next->GetText();

                                Class* ret_type = Class::VOID;
                                Function* func = class_->DefineFunction(ident, ret_type, private_);
                                private_ = false;
                                func->AttachScope(scope_ = new LocalScope(scope_));

                                EXPECT(kLPAREN);
                                ParseParameters(func);
                                EXPECT(kSEMICOLON);

                                scope_ = scope_->GetParent();
                                break;
                            }
                            case kIDENTIFIER:{
                                std::string type = next->GetText();
                                EXPECT(kIDENTIFIER);
                                std::string name = next->GetText();

                                Class* ret_type = code_->FindClass(type);
                                Function* func = class_->DefineFunction(name, ret_type, private_);
                                private_ = false;

                                EXPECT(kLPAREN);
                                ParseParameters(func);
                                EXPECT(kSEMICOLON);
                                break;
                            }
                            case kRBRACE: return class_;
                            default: UNEXPECTED;
                        }
                    } while(true);
                }
                case kCLASS:{
                    EXPECT(kIDENTIFIER);
                    std::string identifier = next->GetText();

                    Class* parent = Class::OBJECT;
                    _c_entry:
                        switch((CONSUME)->GetKind()){
                            case kEXTENDS:{
                                EXPECT(kIDENTIFIER);
                                parent = code_->FindClass(next->GetText());
                                goto _c_entry;
                            }
                            case kLBRACE:{
                                class_ = new Class(identifier, (private_ ? kPrivate : kNone), parent);
                                private_ = false;
                                break;
                            }
                            default: UNEXPECTED;
                        }

                    do{
                        switch((CONSUME)->GetKind()){
                            case kRBRACE: return class_;
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
                                ParseFields();
                                break;
                            }
                            case kIDENTIFIER:{
                                std::string type = next->GetText();
                                EXPECT(kIDENTIFIER);
                                std::string ident = next->GetText();

                                Class* ret_type = code_->FindClass(type);
                                Function* func = class_->DefineFunction(ident, ret_type, private_);
                                private_ = false;

                                EXPECT(kLPAREN);
                                ParseParameters(func);
                                EXPECT(kLBRACE);
                                do{
                                    ParseStatement(func);
                                } while(PeekToken()->GetKind() != kRBRACE);
                                EXPECT(kRBRACE);
                                break;
                            }
                            case kFUNC:{
                                EXPECT(kIDENTIFIER);
                                std::string ident = next->GetText();

                                Class* ret_type = Class::VOID;
                                Function* func = class_->DefineFunction(ident, ret_type, private_);
                                private_ = false;
                                func->AttachScope(scope_ = new LocalScope(scope_));

                                EXPECT(kLPAREN);
                                ParseParameters(func);
                                EXPECT(kLBRACE);
                                do{
                                    ParseStatement(func);
                                } while(PeekToken()->GetKind() != kRBRACE);
                                EXPECT(kRBRACE);

                                scope_ = scope_->GetParent();
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

                    class_ = new Class(identifier, mods, Class::OBJECT, kEnumClass);
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
                                Field* field = class_->DefineStaticField(name, class_, false);
                                Instance* ordinal = new Int(class_->GetFieldCount());
                                class_->GetConstructor()->AddAst(new StoreStaticFieldNode(field, new LiteralNode(ordinal)));
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
                    return class_;
                }
                case kEOF: return class_;
                default: UNEXPECTED;
            }
        } while(class_ != nullptr);
    }

    CodeUnit* Parser::Parse(std::string file){
        file_ = fopen(file.c_str(), "r");
        code_ = new CodeUnit(file);

        while(PeekToken()->GetKind() != kEOF){
            Class* def = ParseDefinition();
            if(def != nullptr) code_->AddClass(def);
        }

        fclose(file_);
        return code_;
    }
}