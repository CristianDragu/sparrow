#pragma once

#include <NodeCommonsH.h>

namespace SprFrontend
{
    /// A function application (function call, or generic instantiation)
    class FunApplication : public Node
    {
        DEFINE_NODE(FunApplication, nkSparrowExpFunApplication, "Sparrow.Exp.FunApplication");

    public:
        FunApplication(const Location& loc, Node* base, NodeList* arguments);
        FunApplication(const Location& loc, Node* base, NodeVector args);

        Node* base() const;
        NodeList* arguments() const;

    protected:
        virtual void doSemanticCheck();

    private:
        void checkStaticCast();
        void checkReinterpretCast();
        void checkSizeOf();
        void checkTypeOf();
        void checkIsValid();
        void checkIsValidAndTrue();
        void checkValueIfValid();
        void checkCtEval();
        void checkLift();
    };
}
