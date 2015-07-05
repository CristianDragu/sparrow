#pragma once

#include "Callable.h"

FWD_CLASS1(Feather, Class);

namespace SprFrontend
{
    using Feather::Class;

    /// A callable object for constructing temporary objects of a class
    class ClassCtorCallable : public Callable
    {
    public:
        ClassCtorCallable(Class* cls, Callable* baseCallable, EvalMode evalMode);

        /// Get the constructors of the given class as callable objects
        static Callables getCtorCallables(Class* cls, EvalMode evalMode);

        virtual const Location& location() const;
        virtual string toString() const;
        virtual size_t paramsCount() const;
        virtual Node* param(size_t idx) const;
        virtual EvalMode evalMode() const;
        virtual bool isAutoCt() const;

        virtual ConversionType canCall(CompilationContext* context, const Location& loc, const vector<TypeRef>& argTypes, EvalMode evalMode, bool noCustomCvt = false);
        virtual ConversionType canCall(CompilationContext* context, const Location& loc, const NodeVector& args, EvalMode evalMode, bool noCustomCvt = false);
        virtual Node* generateCall(const Location& loc);

    private:
        Class* cls_;
        Callable* baseCallable_;    // Function or generic
        EvalMode evalMode_;
        Node* tmpVar_;
        Node* thisArg_;
    };
}
