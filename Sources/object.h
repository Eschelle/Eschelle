#ifndef ESCHELLE_OBJECT_H
#define ESCHELLE_OBJECT_H

#include <string>
#include <sstream>
#include <iostream>
#include "common.h"
#include "array.h"

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
        kNative = 1 << 4
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

        Array<Field*> fields_;
        Array<Function*> functions_;

        friend class Instance;
    public:
        Class(std::string name, int mods, Class* super = Class::OBJECT):
                name_(name),
                mods_(mods),
                fields_(0xA),
                functions_(0xA),
                super_(super){}

        virtual std::string ToString(){
            std::stringstream stream;
            stream << "Class " << name_;
            return stream.str();
        }

        std::string GetName(){
            return name_;
        }

        word GetAllocationSize();

        word GetFieldCount(){
            return fields_.Length();
        }

        Field* DefineField(std::string name, Class* type);
        Field* GetField(std::string name);

        static Class* OBJECT;
        static Class* STRING;
        static Class* BOOLEAN;
        static Class* INTEGER;
        static Class* DOUBLE;
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
    public:
        Field(std::string name, Class* owner, Class* type, int mods):
                name_(name),
                type_(type),
                owner_(owner),
                mods_(mods){}

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
}

#endif //ESCHELLE_OBJECT_H