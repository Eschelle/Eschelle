#ifndef ESCHELLE_OBJECT_H
#define ESCHELLE_OBJECT_H

#include <string>
#include <sstream>
#include <iostream>
#include "common.h"
#include "array.h"
#include "vmemory.h"

namespace Eschelle{
#define FOR_EACH_TYPE(V) \
    V(Object) \
    V(Nil) \
    V(Bool) \
    V(String) \
    V(Byte) \
    V(Int) \
    V(Long) \
    V(Double) \
    V(Class) \
    V(Function) \
    V(Field)

#define HAS_VALUE(Owner, Type) \
    public: \
        static word ValueOffset(){ \
            return OFFSET_OF(Owner, value_); \
        } \
        Type GetValue() const{ \
            return value_; \
        } \
    private: \
        Type value_;

#define FORWARD_DECLARE(Type) class Type;
    FOR_EACH_TYPE(FORWARD_DECLARE)
#undef FORWARD_DECLARE

    class Instance;

    enum Modifier{
        kNone = 1 << 0,
        kFinal = 1 << 1,
        kStatic = 1 << 2,
        kAbstract = 1 << 3,
        kNative = 1 << 4,
        kPrivate = 1 << 5
    };

    enum ClassType{
        kEnumClass = 1 << 0,
        kClassClass = 1 << 1,
        kProtoClass = 1 << 2,
        kObjectClass = 1 << 3
    };

    class Object{
    public:
        virtual std::string ToString() = 0;
    };

    class Class : public Object{
    private:
        std::string name_;
        int mods_;
        Class* super_;
        ClassType type_;

        Array<Field*> fields_;
        Array<Function*> functions_;
        Function* ctor_;

        friend class Instance;
    public:
        Class(std::string name, int mods, Class* super = Class::OBJECT, ClassType type = kClassClass):
                name_(name),
                mods_(mods),
                fields_(0xA),
                functions_(0xA),
                type_(type),
                super_(super){
            if(type != kProtoClass) ctor_ = DefineFunction(name, Class::VOID, false);
        }

        virtual std::string ToString(){
            std::stringstream stream;
            stream << "Class " << name_;
            return stream.str();
        }

        Function* GetConstructor() const{
            return ctor_;
        }

        std::string GetName() const{
            return name_;
        }

        bool IsPrivate() const{
            return (mods_ & kPrivate) == kPrivate;
        }

        bool IsFinal() const{
            return (mods_ & kFinal) == kFinal;
        }

        word GetAllocationSize();

        word GetFieldCount() const{
            return fields_.Length();
        }

        Field* CreateField(std::string name, Class* type, bool priv = false);
        Field* DefineStaticField(std::string name, Class* type, bool priv);
        Field* DefineField(std::string name, Class* type, bool priv);
        Field* GetField(std::string name);

        Field* GetFieldAt(word index) const{
            return fields_[index];
        }

        word GetFunctionCount() const{
            return functions_.Length();
        }

        Function* DefineStaticFunction(std::string name, Class* ret_type, bool priv);
        Function* DefineFunction(std::string name, Class* ret_type, bool priv);
        Function* GetFunction(std::string name);

        Function* GetFunctionAt(word index) const{
            return functions_[index];
        }

        ClassType GetType() const{
            return type_;
        }

        static Class* OBJECT;
        static Class* STRING;
        static Class* BOOLEAN;
        static Class* INTEGER;
        static Class* DOUBLE;
        static Class* VOID;
    };

    class AstNode;
    class SequenceNode;
    class LocalScope;

    class Function : public Object{
    private:
        std::string name_;
        Class* result_type_;
        Class* owner_;
        int mods_;
        MemoryRegion code_;
        SequenceNode* ast_;
    public:
        Function(std::string name, int mods, Class* owner, Class* ret_type);

        std::string GetName() const{
            return name_;
        }

        Class* GetReturnType() const{
            return result_type_;
        }

        Class* GetOwner() const{
            return owner_;
        }

        SequenceNode* GetAst() const{
            return ast_;
        }

        void AttachScope(LocalScope* scope);

        void AddAst(AstNode* node);

        virtual std::string ToString(){
            std::stringstream stream;
            stream << "Function " << name_;
            return stream.str();
        }

        bool IsAbstract() const{
            return GetOwner()->GetType() == kProtoClass;
        }
    };

    class Field : public Object{
    private:
        std::string name_;
        Class* type_;
        Class* owner_;
        int mods_;

        union{
            word offset_;
            Instance* static_;
        };

        friend class Parser;
    public:
        Field(std::string name, Class* owner, Class* type, int mods):
                name_(name),
                type_(type),
                owner_(owner),
                mods_(mods){}

        AstNode* CreateStore(AstNode* value);
        AstNode* CreateLoad();

        std::string GetName() const{
            return name_;
        }

        Class* GetFieldType() const{
            return type_;
        }

        bool IsStatic() const{
            return (mods_ & kStatic) == kStatic;
        }

        bool IsFinal() const{
            return (mods_ & kFinal) == kFinal;
        }

        bool IsPrivate() const{
            return (mods_ & kPrivate) == kPrivate;
        }

        word GetOffset(){
            return offset_;
        }

        void SetOffset(word off){
            offset_ = off;
        }

        Instance* GetStaticValue() {
            return static_;
        }

        virtual std::string ToString(){
            return "Field";
        }
    };

    class Instance : public Object{
    private:
        Class* type_;

        Instance** FieldAddrAtOffset(word offset) const{
            return reinterpret_cast<Instance**>(reinterpret_cast<uword>(this) + offset);
        }

        inline Instance** FieldAddr(Field* field) const{
            return FieldAddrAtOffset(field->GetOffset());
        }
    protected:
        Instance(Class* type):
                type_(type){}
    public:
        Class* GetType() const{
            return type_;
        }

        static Instance* New(Class* type);

        static word TypeOffset(){
            return OFFSET_OF(Instance, type_);
        }

        void SetField(Field* field, Instance* value){
            *FieldAddr(field) = value;
        }

        Instance* GetField(Field* field){
            return *FieldAddr(field);
        }

        virtual std::string ToString(){
            std::stringstream sstream;
            sstream << "Instance[" + type_->ToString() << "]";
            return sstream.str();
        }
    };

    class String : public Instance{
    public:
        String(std::string value):
                value_(value),
                Instance(Class::STRING){}

        virtual std::string ToString(){
            return value_;
        }

        HAS_VALUE(String, std::string);
    };

    class Bool : public Instance{
    private:
        Bool(bool value):
                value_(value),
                Instance(Class::BOOLEAN){}
    public:
        virtual std::string ToString(){
            return value_ ? "true" : "false";
        }

        HAS_VALUE(Bool, bool);
    public:
        static Bool* FALSE;
        static Bool* TRUE;
    };

    class Int : public Instance{
    public:
        Int(int value):
                value_(value),
                Instance(Class::INTEGER){}

        virtual std::string ToString(){
            std::stringstream stream;
            stream << value_;
            return stream.str();
        }

        HAS_VALUE(Int, int);
    };

    class Double : public Instance{
    public:
        Double(double value):
                value_(value),
                Instance(Class::DOUBLE){}

        virtual std::string ToString(){
            std::stringstream stream;
            stream << value_;
            return stream.str();
        }

        HAS_VALUE(Double, double);
    };

    class CodeUnit{
    private:
        std::string location_;
        Array<Class*> classes_;

        void AddClass(Class* cls){
            classes_.Add(cls);
        }

        friend class Parser;
    public:
        CodeUnit(std::string loc):
                classes_(10),
                location_(loc){
            AddClass(Class::STRING);
            AddClass(Class::INTEGER);
            AddClass(Class::DOUBLE);
            AddClass(Class::BOOLEAN);
        }

        Class* FindClass(std::string name) const{
            for(int i = 0; i < classes_.Length(); i++){
                if(classes_[i]->GetName() == name) return classes_[i];
            }
            return nullptr;
        }

        word GetClassesSize() const{
            return classes_.Length();
        }

        Class* GetClassAt(word index) const{
            return classes_[index];
        }
    };

#if defined(ESCH_DEBUG)
    std::ostream& operator<<(std::ostream& stream, const Class& cls);
#endif // ESCH_DEBUG
}

#endif //ESCHELLE_OBJECT_H