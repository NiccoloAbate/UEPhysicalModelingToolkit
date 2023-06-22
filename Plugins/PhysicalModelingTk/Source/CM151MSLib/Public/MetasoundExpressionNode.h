#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

#include <array>
#include <string>

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

    namespace FExpressionOperatorPrivate
    {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        static constexpr int num_te_functions = 3;
#endif
    }

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_Expression"

    namespace ExpressionNode
    {
        METASOUND_PARAM(InParamNameExpr, "Expr", "String input.")
        METASOUND_PARAM(InParamNameData, "X", "Float input.")
        METASOUND_PARAM(OutParamNameData, "Out", "Float output.")
        METASOUND_PARAM(OutParamNameErrorTrigger, "OnError", "Triggers on error.")
        METASOUND_PARAM(OutParamNameErrorMessage, "ErrorMsg", "Error message.")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FExpressionOperator
    //------------------------------------------------------------------------------------
    template <typename ValueType, uint32 NumArgs>
    class FExpressionOperator : public TExecutableOperator<FExpressionOperator<ValueType, NumArgs>>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FExpressionOperator(const FOperatorSettings& InSettings, const FStringReadRef& InExprInput, TArray<TDataReadReference<ValueType>>&& InDataInput);
        virtual ~FExpressionOperator();

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FStringReadRef ExprInput;
        TArray<TDataReadReference<ValueType>> DataInputs;
        TDataWriteReference<ValueType> DataOutput;
        FTriggerWriteRef ErrorTriggerOutput;
        FStringWriteRef ErrorMessageOutput;

        static constexpr bool LOG_COMPILE_ERRORS = true;

#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        FString LastExprString;
        bool bNeedsToCompileExpression = false;
        std::array<double, NumArgs> X;
        std::array<std::string, NumArgs> VarNames;
        static constexpr int nBindings = NumArgs + FExpressionOperatorPrivate::num_te_functions;
        std::array<te_variable, nBindings> Vars;
        te_expr* Expr = NULL;
#endif
    };

    //------------------------------------------------------------------------------------
    // FExpressionNode
    //------------------------------------------------------------------------------------
    template <typename ValueType, uint32 NumArgs>
    class FExpressionNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FExpressionNode(const FNodeInitData& InitData);
    };
}
