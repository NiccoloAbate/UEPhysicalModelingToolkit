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

// use of std functions seems to cause errors, I think do to the references?
//#include <algorithm>
//#define TINYEXPR_MIN static_cast<const double& (*)(const double&, const double&)>(&std::min<double>)
//#define TINYEXPR_MAX static_cast<const double& (*)(const double&, const double&)>(&std::max<double>)
//#define TINYEXPR_CLAMP static_cast<const double& (*)(const double&, const double&, const double&)>(&std::clamp<double>)

double te_min(double a, double b) { return a < b ? a : b; }
double te_max(double a, double b) { return a > b ? a : b; }
double te_clamp(double x, double min, double max) { return te_min(te_max(x, min), max); }
#define TINYEXPR_MIN te_min
#define TINYEXPR_MAX te_max
#define TINYEXPR_CLAMP te_clamp
#endif

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_Expression"

    namespace ExpressionFloatNode
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
    template <typename ValueType>
    class FExpressionOperator : public TExecutableOperator<FExpressionOperator<ValueType>>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FExpressionOperator(const FOperatorSettings& InSettings, const FStringReadRef& InStringInput, const TDataReadReference<ValueType>& InFloatInput);
        virtual ~FExpressionOperator();

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FStringReadRef ExprInput;
        TDataReadReference<ValueType>  DataInput;
        TDataWriteReference<ValueType> DataOutput;
        FTriggerWriteRef ErrorTriggerOutput;
        FStringWriteRef ErrorMessageOutput;

        static constexpr bool LOG_COMPILE_ERRORS = true;

#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        FString LastExprString;
        double X;
        static constexpr int nBindings = 4;
        te_variable Vars[nBindings] = {
            { "x", &X, TE_VARIABLE },
            { "min", &TINYEXPR_MIN, TE_FUNCTION2 },
            { "max", &TINYEXPR_MAX, TE_FUNCTION2 },
            { "clamp", &TINYEXPR_CLAMP, TE_FUNCTION3 },
        };
        te_expr* Expr = NULL;
#endif
    };

    //------------------------------------------------------------------------------------
    // FExpressionNode
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    class FExpressionNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FExpressionNode(const FNodeInitData& InitData);
    };
}