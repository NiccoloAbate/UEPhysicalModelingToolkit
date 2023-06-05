#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

#define EXPRESSIONLIB_NONE 0
#define EXPRESSIONLIB_EXPRTK 1
#define EXPRESSIONLIB_TINYEXPR 2
#define EXPRESSIONLIB EXPRESSIONLIB_TINYEXPR

#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK
#pragma push_macro("check")   // store 'check' macro current definition
#undef check  // undef to avoid conflicts
#pragma push_macro("verify")   // store 'verify' macro current definition
#undef verify  // undef to avoid conflicts
#include "exprtk/exprtk.hpp"
#pragma pop_macro("check")  // restore definition
#pragma pop_macro("verify")  // restore definition
#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
#include "tinyexpr/tinyexpr.h"
#endif

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_ExpressionFloat"

    namespace ExpressionFloatNode
    {
        METASOUND_PARAM(InParamNameStringInput, "Expr", "String input.")
        METASOUND_PARAM(InParamNameFloatInput, "X", "Float input.")
        METASOUND_PARAM(OutParamNameFloatOutput, "Out", "Float output.")
        METASOUND_PARAM(OutParamNameTriggerOutput, "OnError", "Triggers on error.")
        METASOUND_PARAM(OutParamNameStringOutput, "ErrorMsg", "Error message.")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FExpressionFloatOperator
    //------------------------------------------------------------------------------------
    class FExpressionFloatOperator : public TExecutableOperator<FExpressionFloatOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FExpressionFloatOperator(const FOperatorSettings& InSettings, const FStringReadRef& InStringInput, const FFloatReadRef& InFloatInput);
        virtual ~FExpressionFloatOperator();

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FStringReadRef StringInput;
        FFloatReadRef  FloatInput;
        FFloatWriteRef FloatOutput;
        FTriggerWriteRef TriggerOutput;
        FStringWriteRef StringOutput;

#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        FString LastExprString;
        double X;
        te_variable Vars[1] = { { "x", &X } };
        te_expr* Expr = NULL;
#endif
    };

    //------------------------------------------------------------------------------------
    // FExpressionFloatNode
    //------------------------------------------------------------------------------------
    class FExpressionFloatNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FExpressionFloatNode(const FNodeInitData& InitData);
    };
}
