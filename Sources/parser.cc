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
        AstNode* primary = nullptr;

        Token* next;
        switch((CONSUME)->GetKind()){
            case kLIT_NUMBER:{
                return new LiteralNode(new Int(atoi(next->GetText().c_str())));
            }
            case kLIT_STRING:{
                return new LiteralNode(new String(next->GetText()));
            }
            default:{
                UNEXPECTED;
            }
        }

        return primary;
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

    void Parser::ParseStatement(Class* cls, Function* func){
        Token* next;
        switch((CONSUME)->GetKind()){
            case kSEMICOLON: return;
            case kVAR:{
                Array<LocalDesc*>* locals = ParseLocals();
                for(int i = 0; i < locals->Length(); i++){
                    LocalVariable* local = new LocalVariable((*locals)[i]->type, (*locals)[i]->name);
                    if(!func->GetAst()->GetScope()->AddLocal(local)){
                        std::cerr << "Unable to add local " << (*locals)[i]->name << std::endl;
                        getchar();
                        abort();
                    }
                    if((*locals)[i]->value != nullptr){
                        func->AddAst(new StoreLocalNode(local, (*locals)[i]->value));
                    }
                }
                break;
            }
            default:{
                UNEXPECTED;
            }
        }
    }

    Array<Parser::LocalDesc*>* Parser::ParseLocals(){
        Array<LocalDesc*>* locals = new Array<LocalDesc*>(10);

        Token* next = nullptr;

        entry:
            EXPECT(kIDENTIFIER);
            std::string ident = next->GetText();
            EXPECT(kCOLON);
            EXPECT(kIDENTIFIER);
            std::string type_ident = next->GetText();
            Class* type_cls = code_->FindClass(type_ident);

            AstNode* value = nullptr;
        exit_0:
            switch((CONSUME)->GetKind()){
                case kCOMMA:{
                    goto exit_1;
                }
                case kSEMICOLON:{
                    locals->Add(new LocalDesc(ident, type_cls, value));
                    return locals;
                }
                case kEQUALS:{
                    value = ParseBinaryExpr();
                    goto exit_0;
                }
                default: UNEXPECTED;
            }
        exit_1:
            locals->Add(new LocalDesc(ident, type_cls, value));
            goto entry;
    }

    Array<Parser::FieldDesc*>* Parser::ParseFields(bool parse_init){
        Array<FieldDesc*>* fields = new Array<FieldDesc*>(10);

        Token* next = nullptr;

        entry:
            EXPECT(kIDENTIFIER);
            std::string ident = next->GetText();
            EXPECT(kCOLON);
            EXPECT(kIDENTIFIER);
            std::string type_ident = next->GetText();
            Class* type_cls = code_->FindClass(type_ident);

            AstNode* value = nullptr;
        exit_0:
            switch((CONSUME)->GetKind()){
                case kCOMMA:{
                    goto exit_1;
                }
                case kSEMICOLON:{
                    fields->Add(new FieldDesc(ident, type_cls, value));
                    return fields;
                }
                case kEQUALS:{
                    if(!parse_init) UNEXPECTED;
                    value = ParseBinaryExpr();
                    goto exit_0;
                }
                default: UNEXPECTED;
            }
        exit_1:
            fields->Add(new FieldDesc(ident, type_cls, value));
            goto entry;
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
                case kPROTO:{
                    EXPECT(kIDENTIFIER);
                    std::string cls_ident = next->GetText();

                    Class* parent = Class::OBJECT;
                    _p_entry:
                        switch((CONSUME)->GetKind()){
                            case kLBRACE:{
                                result = new Class(cls_ident, (private_ ? kPrivate : kNone), parent, kProtoClass);
                                private_ = false;
                                break;
                            }
                            default: UNEXPECTED;
                        }

                    do{
                        switch((CONSUME)->GetKind()){
                            case kVAR:{
                                std::cout << "Parsing fields" << std::endl;
                                Array<FieldDesc*>* fields = ParseFields(false);
                                for(int i = 0; i < fields->Length(); i++){
                                    std::cout << "Adding field " << (*fields)[i]->name << std::endl;
                                    if(result->GetField((*fields)[i]->name) != nullptr){
                                        std::cerr << "Unable to add field '" << (*fields)[i]->name << "'" << std::endl;
                                        getchar();
                                        abort();
                                    }

                                    Field* field = result->DefineStaticField((*fields)[i]->name, (*fields)[i]->type, private_);
                                    if((*fields)[i]->value != nullptr) result->GetConstructor()->AddAst(new StoreStaticFieldNode(field, (*fields)[i]->value));
                                }
                                break;
                            }
                            case kFUNC:{
                                EXPECT(kIDENTIFIER);
                                std::string ident = next->GetText();

                                Class* ret_type = Class::VOID;
                                Function* func = result->DefineFunction(ident, ret_type, private_);

                                EXPECT(kLPAREN);
                                ParseParameters(func);
                                EXPECT(kSEMICOLON);
                                break;
                            }
                            case kRBRACE: return result;
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
                                result = new Class(identifier, (private_ ? kPrivate : kNone), parent);
                                private_ = false;
                                break;
                            }
                            default: UNEXPECTED;
                        }

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
                                Array<FieldDesc*>* fields = ParseFields(true);
                                for(int i = 0; i < fields->Length(); i++){
                                    if(result->GetField((*fields)[i]->name) != nullptr){
                                        std::cerr << "Unable to add field '" << (*fields)[i]->name << "'" << std::endl;
                                        getchar();
                                        abort();
                                    }
                                    Field* f = result->DefineField((*fields)[i]->name, (*fields)[i]->type, private_);
                                    if((*fields)[i]->value != nullptr){
                                        result->GetConstructor()->AddAst(new StoreInstanceFieldNode(f, (*fields)[i]->value));
                                    }
                                }
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
                                do{
                                    ParseStatement(result, func);
                                } while(PeekToken()->GetKind() != kRBRACE);
                                EXPECT(kRBRACE);
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

                    result = new Class(identifier, mods, Class::OBJECT, kEnumClass);
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
                                Field* field = result->DefineStaticField(name, result, false);
                                Object* ordinal = new Int(result->GetFieldCount());
                                result->GetConstructor()->AddAst(new StoreStaticFieldNode(field, new LiteralNode(ordinal)));
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

        while(PeekToken()->GetKind() != kEOF){
            Class* def = ParseDefinition();
            if(def != nullptr) code_->AddClass(def);
        }

        fclose(file_);
        return code_;
    }
}