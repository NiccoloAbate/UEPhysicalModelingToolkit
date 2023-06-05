#include "MetasoundExpressionNode.h"

#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK || EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
#include <string>
#endif

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_ExpressionNode"

namespace Metasound
{
    namespace FExpressionOperatorPrivate
    {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK || EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        void ToLower(std::string& s)
        {
            std::transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c) { return std::tolower(c); });
        }
#endif

        template <typename ValueType>
        struct TExpression
        {
            bool bIsSupported = false;
        };

        template <>
        struct TExpression<float>
        {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
            static void SampleExpression(te_expr* Expr, double& X, const float& In, float& Out)
            {
                X = (double)In;
                Out = (float)te_eval(Expr);
            }
#endif

            static TDataReadReference<float> CreateInRefData(const FCreateOperatorParams& InParams)
            {
                using namespace ExpressionFloatNode;

                const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();

                return InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameData), InParams.OperatorSettings);
            }
        };

        template <>
        struct TExpression<FAudioBuffer>
        {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
            static void SampleExpression(te_expr* Expr, double& X, const FAudioBuffer& In, FAudioBuffer& Out)
            {
                const float* InAudio = In.GetData();
                float* OutAudio = Out.GetData();

                const int NumSamples = In.Num();
                for (int i = 0; i < NumSamples; ++i)
                {
                    X = (double)InAudio[i];
                    OutAudio[i] = (float)te_eval(Expr);
                }
            }
#endif

            static TDataReadReference<FAudioBuffer> CreateInRefData(const FCreateOperatorParams& InParams)
            {
                using namespace ExpressionFloatNode;

                const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();

                return InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameData), InParams.OperatorSettings);
            }
        };
    }

    //------------------------------------------------------------------------------------
    // FExpressionOperator
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    FExpressionOperator<ValueType>::FExpressionOperator(const FOperatorSettings& InSettings, const FStringReadRef& InStringInput, const TDataReadReference<ValueType>& InFloatInput)
        : ExprInput(InStringInput)
        , DataInput(InFloatInput)
        , DataOutput(TDataWriteReferenceFactory<ValueType>::CreateAny(InSettings))
        , ErrorTriggerOutput(FTriggerWriteRef::CreateNew(InSettings))
        , ErrorMessageOutput(TDataWriteReferenceFactory<FString>::CreateAny(InSettings))
    {
    }

    template <typename ValueType>
    FExpressionOperator<ValueType>::~FExpressionOperator()
    {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        if (Expr)
            te_free(Expr);
#endif
    }

    template <typename ValueType>
    FDataReferenceCollection FExpressionOperator<ValueType>::GetInputs() const
    {
        using namespace ExpressionFloatNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameExpr), ExprInput);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameData), DataInput);

        return InputDataReferences;
    }

    template <typename ValueType>
    FDataReferenceCollection FExpressionOperator<ValueType>::GetOutputs() const
    {
        using namespace ExpressionFloatNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameData), DataOutput);
        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameErrorTrigger), ErrorTriggerOutput);
        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameErrorMessage), ErrorMessageOutput);


        return OutputDataReferences;
    }

    template <typename ValueType>
    void FExpressionOperator<ValueType>::Execute()
    {
        const ValueType* InputData = DataInput.Get();
        const FString* InputExpr = ExprInput.Get();
        ValueType* OutputData = DataOutput.Get();
        FString* OutputErrorMessage = ErrorMessageOutput.Get();
        FTrigger* OutputErrorTrigger = ErrorTriggerOutput.Get();

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

        *OutputFloat = expression.value();
#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        // if the expression string has changed, free Expr
        if (*InputExpr == LastExprString)
        {
            if (Expr)
            {
                te_free(Expr);
                Expr = NULL;
            }

            LastExprString = *InputExpr;
        }

        // if needs to compile
        if (Expr == NULL)
        {
            std::string expression = std::string(TCHAR_TO_UTF8(**InputExpr));
            FExpressionOperatorPrivate::ToLower(expression);

            int err;
            // Compile the expression with variables.
            Expr = te_compile(expression.c_str(), Vars, 1, &err);
            // Output error for failed compilation
            if (!Expr)
            {
                if constexpr (LOG_COMPILE_ERRORS)
                    UE_LOG(LogTemp, Log, TEXT("Error: Invalid expression; error at char %d"), err);
                *OutputErrorMessage = "Invalid expression; error at char " + FString::FromInt(err);
                OutputErrorTrigger->TriggerFrame(0);
            }
        }

        // if successfully compiled expression
        if (Expr)
        {
            FExpressionOperatorPrivate::TExpression<ValueType>::SampleExpression(Expr, X, *InputData, *OutputData);
        }
#else
        *OutputFloat = *InputFloat;
#endif
    }

    template <typename ValueType>
    const FVertexInterface& FExpressionOperator<ValueType>::GetVertexInterface()
    {
        using namespace ExpressionFloatNode;

        static const FVertexInterface Interface(
            FInputVertexInterface(
                TInputDataVertex<FString>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameExpr)),
                TInputDataVertex<ValueType>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameData))
            ),

            FOutputVertexInterface(
                TOutputDataVertex<ValueType>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameData)),
                TOutputDataVertex<FTrigger>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameErrorTrigger)),
                TOutputDataVertex<FString>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameErrorMessage))
            )
        );

        return Interface;
    }

    template <typename ValueType>
    const FNodeClassMetadata& FExpressionOperator<ValueType>::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName = { TEXT("UE"), TEXT("Expression"), GetMetasoundDataTypeName<ValueType>() };
            Info.MajorVersion = 1;
            Info.MinorVersion = 0;
            Info.DisplayName = METASOUND_LOCTEXT_FORMAT("Metasound_ExpressionDisplayName", "Expression ({0})", GetMetasoundDataTypeDisplayText<ValueType>());
            Info.Description = LOCTEXT("Metasound_ExpressionNodeDescription", "Computes expression for X.");
            Info.Author = PluginAuthor;
            Info.PromptIfMissing = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_ExpressionNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    template <typename ValueType>
    TUniquePtr<IOperator> FExpressionOperator<ValueType>::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace ExpressionFloatNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface = GetVertexInterface().GetInputInterface();

        FStringReadRef ExprIn = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FString>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameExpr), InParams.OperatorSettings);
        TDataReadReference<ValueType> DataIn = FExpressionOperatorPrivate::TExpression<ValueType>::CreateInRefData(InParams);

        return MakeUnique<FExpressionOperator>(InParams.OperatorSettings, ExprIn, DataIn);
    }


    //------------------------------------------------------------------------------------
    // FExpressionNode
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    FExpressionNode<ValueType>::FExpressionNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FExpressionOperator<ValueType>>())
    {

    }

    using FExpressionNodeFloat = FExpressionNode<float>;
    METASOUND_REGISTER_NODE(FExpressionNodeFloat)

        using FExpressionNodeAudio = FExpressionNode<FAudioBuffer>;
    METASOUND_REGISTER_NODE(FExpressionNodeAudio)

    template class FExpressionNode<float>;
    template class FExpressionNode<FAudioBuffer>;
}

#undef LOCTEXT_NAMESPACE
