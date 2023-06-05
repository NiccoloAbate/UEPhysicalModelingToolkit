#include "MetasoundExpressionFloatNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_ExpressionFloatNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FExpressionFloatOperator
    //------------------------------------------------------------------------------------
    FExpressionFloatOperator::FExpressionFloatOperator(const FOperatorSettings& InSettings, const FStringReadRef& InStringInput, const FFloatReadRef& InFloatInput)
        : StringInput(InStringInput)
        , FloatInput(InFloatInput)
        , FloatOutput(TDataWriteReferenceFactory<float>::CreateAny(InSettings))
        , TriggerOutput(FTriggerWriteRef::CreateNew(InSettings))
        , StringOutput(TDataWriteReferenceFactory<FString>::CreateAny(InSettings))
    {
    }

    FExpressionFloatOperator::~FExpressionFloatOperator()
    {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        if (Expr)
            te_free(Expr);
#endif
    }

    FDataReferenceCollection FExpressionFloatOperator::GetInputs() const
    {
        using namespace ExpressionFloatNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameStringInput), StringInput);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameFloatInput), FloatInput);

        return InputDataReferences;
    }

    FDataReferenceCollection FExpressionFloatOperator::GetOutputs() const
    {
        using namespace ExpressionFloatNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameFloatOutput), FloatOutput);
        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameTriggerOutput), TriggerOutput);
        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameStringOutput), StringOutput);


        return OutputDataReferences;
    }

    void FExpressionFloatOperator::Execute()
    {
        const float* InputFloat = FloatInput.Get();
        const FString* InputString = StringInput.Get();
        float* OutputFloat = FloatOutput.Get();
        FString* OutputString = StringOutput.Get();
        FTrigger* OutputTrigger = TriggerOutput.Get();

        float Value;

#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK
        const std::string expression_string = std::string(TCHAR_TO_UTF8(**InputString));

        typedef exprtk::symbol_table<float> symbol_table_t;
        typedef exprtk::expression<float>   expression_t;
        typedef exprtk::parser<float>       parser_t;

        float x = *InputFloat;

        symbol_table_t symbol_table;
        symbol_table.add_variable("x", x);
        symbol_table.add_constants();

        expression_t expression;
        expression.register_symbol_table(symbol_table);

        parser_t parser;
        parser.compile(expression_string, expression);

        Value = expression.value();
#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        // if the expression string has changed, free Expr
        if (*InputString == LastExprString)
        {
            if (Expr)
            {
                te_free(Expr);
                Expr = NULL;
            }

            LastExprString = *InputString;
        }

        // if needs to compile
        if (Expr == NULL)
        {
            const std::string expression = std::string(TCHAR_TO_UTF8(**InputString));

            int err;
            // Compile the expression with variables.
            Expr = te_compile(expression.c_str(), Vars, 1, &err);
            // Output error for failed compilation
            if (!Expr)
            {
                UE_LOG(LogTemp, Log, TEXT("Error: Invalid expression; error at char %d"), err);
                *OutputString = "Invalid expression; error at char " + FString::FromInt(err);
                OutputTrigger->TriggerFrame(0);
            }
        }

        // if successfully compiled expression
        if (Expr)
        {
            X = (double)*InputFloat;
            Value = (float)te_eval(Expr);
        }

        /*
        const std::string expression = std::string(TCHAR_TO_UTF8(**InputString));
        double x = (double)*InputFloat; // must be double for tinyexpr it seems
        // Store variable names and pointers.
        te_variable vars[] = { {"x", &x} };

        int err;
        // Compile the expression with variables.
        te_expr* expr = te_compile(expression.c_str(), vars, 1, &err);

        if (expr) {
            Value = (float)te_eval(expr);

            te_free(expr);
        }
        else {
            UE_LOG(LogTemp, Log, TEXT("Error: Invalid expression; error at char %d"), err);
            *OutputString = "Invalid expression; error at char " + FString::FromInt(err);
            OutputTrigger->TriggerFrame(0);
        }
        */
#else
        Value = *InputFloat;
#endif

        *OutputFloat = Value;
    }

    const FVertexInterface& FExpressionFloatOperator::GetVertexInterface()
    {
        using namespace ExpressionFloatNode;

        static const FVertexInterface Interface(
            FInputVertexInterface(
                TInputDataVertex<FString>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameStringInput)),
                TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameFloatInput))
            ),

            FOutputVertexInterface(
                TOutputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameFloatOutput)),
                TOutputDataVertex<FTrigger>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameTriggerOutput)),
                TOutputDataVertex<FString>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameStringOutput))
            )
        );

        return Interface;
    }

    const FNodeClassMetadata& FExpressionFloatOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName = { TEXT("UE"), TEXT("ExpressionFloat"), TEXT("Audio") };
            Info.MajorVersion = 1;
            Info.MinorVersion = 0;
            Info.DisplayName = LOCTEXT("Metasound_ExpressionFloatDisplayName", "Expression (Float)");
            Info.Description = LOCTEXT("Metasound_ExpressionFloatNodeDescription", "Computes expression for X.");
            Info.Author = PluginAuthor;
            Info.PromptIfMissing = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_ExpressionFloatNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FExpressionFloatOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace ExpressionFloatNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface = GetVertexInterface().GetInputInterface();

        FStringReadRef StringIn = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FString>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameStringInput), InParams.OperatorSettings);
        FFloatReadRef FloatIn = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameFloatInput), InParams.OperatorSettings);

        return MakeUnique<FExpressionFloatOperator>(InParams.OperatorSettings, StringIn, FloatIn);
    }


    //------------------------------------------------------------------------------------
    // FExpressionFloatNode
    //------------------------------------------------------------------------------------
    FExpressionFloatNode::FExpressionFloatNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FExpressionFloatOperator>())
    {

    }

    METASOUND_REGISTER_NODE(FExpressionFloatNode)
}

#undef LOCTEXT_NAMESPACE
