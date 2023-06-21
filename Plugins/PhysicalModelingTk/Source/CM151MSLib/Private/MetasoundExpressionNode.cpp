#include "MetasoundExpressionNode.h"

#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK || EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
#include <string>
#include <algorithm>
#endif

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_ExpressionNode"

#define REGISTER_EXPRESSION_NODE(DataType, Number) \
    using FExpressionNode##DataType##Number = FExpressionNode<DataType, Number>;      \
    METASOUND_REGISTER_NODE(FExpressionNode##DataType##Number)                        \
    template class FExpressionNode<DataType, Number>;

namespace Metasound
{
    namespace ExpressionNode
    {
        const FVertexName GetInputDataName(uint32 InIndex)
        {
            if (InIndex == 0)
                return TEXT("X");

            return *FString::Format(TEXT("X{0}"), { InIndex });
        }
        const FText GetInputDataDescription(uint32 InIndex)
        {
            if (InIndex == 0)
                return METASOUND_LOCTEXT("ExpressionDataDesc", "Input argument X / X0.");

            return METASOUND_LOCTEXT_FORMAT("ExpressionDataDesc", "Input argument X{0}.", InIndex);
        }
        const FText GetInputDataDisplayName(uint32 InIndex)
        {
            if (InIndex == 0)
                return METASOUND_LOCTEXT("ExpressionDataDisplayName", "X");

            return METASOUND_LOCTEXT_FORMAT("ExpressionDataDisplayName", "X{0}", InIndex);
        }
        const FText GetNodeDescription(uint32 NumArgs)
        {
            switch (NumArgs)
            {
            case 0:
                return LOCTEXT("Metasound_ExpressionNodeDescription", "Computes expression");
            case 1:
                return LOCTEXT("Metasound_ExpressionNodeDescription", "Computes expression for X / X0");
            case 2:
                return LOCTEXT("Metasound_ExpressionNodeDescription", "Computes expression for X / X0, X1");
            default:
                return LOCTEXT("Metasound_ExpressionNodeDescription", "Computes expression for X / X0, X1, ...");
            }
        }
    }

    namespace FExpressionOperatorPrivate
    {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR

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

        te_variable te_functions[num_te_functions] = {
            { "min", &TINYEXPR_MIN, TE_FUNCTION2 },
            { "max", &TINYEXPR_MAX, TE_FUNCTION2 },
            { "clamp", &TINYEXPR_CLAMP, TE_FUNCTION3 },
        };
#endif

#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK || EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        void ToLower(std::string& s)
        {
            std::transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c) { return std::tolower(c); });
        }

        void ReplaceAll(std::string& s, std::string const& toReplace, std::string const& replaceWith)
        {
            std::string buf;
            std::size_t pos = 0;
            std::size_t prevPos;

            // Reserves rough estimate of final size of string.
            buf.reserve(s.size());

            while (true) {
                prevPos = pos;
                pos = s.find(toReplace, pos);
                if (pos == std::string::npos)
                    break;
                buf.append(s, prevPos, pos - prevPos);
                buf += replaceWith;
                pos += toReplace.size();
            }

            buf.append(s, prevPos, s.size() - prevPos);
            s.swap(buf);
        }

        void FormatExpr(std::string& s)
        {
            ToLower(s);
            ReplaceAll(s, "x0", "x");
        }
#endif

        template <typename ValueType, uint32 NumArgs>
        struct TExpression
        {
            bool bIsSupported = false;
        };

        template <uint32 NumArgs>
        struct TExpression<float, NumArgs>
        {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
            static void SampleExpression(te_expr* Expr, std::array<double, NumArgs>& X, TArray<TDataReadReference<float>>& In, TDataWriteReference<float>& Out)
            {
                // couple samples into variable buffer X
                for (int i = 0; i < X.size(); ++i)
                    X[i] = (double)(*In[i].Get());
                *Out.Get() = (float)te_eval(Expr);
            }
#endif

            static TDataReadReference<float> CreateInRefData(const FCreateOperatorParams& InParams, uint32 InIndex)
            {
                using namespace ExpressionNode;

                const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();

                return InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, GetInputDataName(InIndex), InParams.OperatorSettings);
            }
        };

        template <uint32 NumArgs>
        struct TExpression<FAudioBuffer, NumArgs>
        {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
            static void SampleExpression(te_expr* Expr, std::array<double, NumArgs>& X, TArray<TDataReadReference<FAudioBuffer>>& In, TDataWriteReference<FAudioBuffer>& Out)
            {
                std::array<const float*, NumArgs> InAudio;
                for (int i = 0; i < NumArgs; ++i)
                    InAudio[i] = In[i].Get()->GetData();
                float* OutAudio = Out.Get()->GetData();

                const int NumSamples = Out.Get()->Num();
                for (int i = 0; i < NumSamples; ++i)
                {
                    // couple samples into variable buffer X
                    for (int j = 0; j < X.size(); ++j)
                        X[j] = (double)(InAudio[j][i]);

                    OutAudio[i] = (float)te_eval(Expr);
                }
            }
#endif

            static TDataReadReference<FAudioBuffer> CreateInRefData(const FCreateOperatorParams& InParams, uint32 InIndex)
            {
                using namespace ExpressionNode;

                const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();

                return InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(GetInputDataName(InIndex), InParams.OperatorSettings);
            }
        };
    }

    //------------------------------------------------------------------------------------
    // FExpressionOperator
    //------------------------------------------------------------------------------------
    template <typename ValueType, uint32 NumArgs>
    FExpressionOperator<ValueType, NumArgs>::FExpressionOperator(const FOperatorSettings& InSettings, const FStringReadRef& InExprInput, TArray<TDataReadReference<ValueType>>&& InDataInput)
        : ExprInput(InExprInput)
        , DataInputs(MoveTemp(InDataInput))
        , DataOutput(TDataWriteReferenceFactory<ValueType>::CreateAny(InSettings))
        , ErrorTriggerOutput(FTriggerWriteRef::CreateNew(InSettings))
        , ErrorMessageOutput(TDataWriteReferenceFactory<FString>::CreateAny(InSettings))
    {
        // bind variables
        for (int i = 0; i < NumArgs; ++i)
        {
            // store var name strings in memory
            VarNames[i] = std::string(TCHAR_TO_UTF8(*ExpressionNode::GetInputDataName(i).ToString().ToLower()));
            // construct variable bindings
            Vars[i] = { VarNames[i].c_str(), &X[i], TE_VARIABLE};
        }
        // copy function bindings into Vars
        std::copy(FExpressionOperatorPrivate::te_functions, FExpressionOperatorPrivate::te_functions + FExpressionOperatorPrivate::num_te_functions, Vars.begin() + NumArgs);
    }

    template <typename ValueType, uint32 NumArgs>
    FExpressionOperator<ValueType, NumArgs>::~FExpressionOperator()
    {
#if EXPRESSIONLIB == EXPRESSIONLIB_EXPRTK

#elif EXPRESSIONLIB == EXPRESSIONLIB_TINYEXPR
        if (Expr)
            te_free(Expr);
#endif
    }

    template <typename ValueType, uint32 NumArgs>
    FDataReferenceCollection FExpressionOperator<ValueType, NumArgs>::GetInputs() const
    {
        using namespace ExpressionNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameExpr), ExprInput);
        for (uint32 i = 0; i < NumArgs; ++i)
            InputDataReferences.AddDataReadReference(GetInputDataName(i), DataInputs[i]);

        return InputDataReferences;
    }

    template <typename ValueType, uint32 NumArgs>
    FDataReferenceCollection FExpressionOperator<ValueType, NumArgs>::GetOutputs() const
    {
        using namespace ExpressionNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameData), DataOutput);
        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameErrorTrigger), ErrorTriggerOutput);
        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameErrorMessage), ErrorMessageOutput);


        return OutputDataReferences;
    }

    template <typename ValueType, uint32 NumArgs>
    void FExpressionOperator<ValueType, NumArgs>::Execute()
    {
        const FString* InputExpr = ExprInput.Get();
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
        // if the expression string has changed, free Expr and mark ready for recompilation
        if (*InputExpr != LastExprString)
        {
            if (Expr)
            {
                te_free(Expr);
                Expr = NULL;
            }

            LastExprString = *InputExpr;
            bNeedsToCompileExpression = true;
        }

        // if needs to compile
        if (bNeedsToCompileExpression)
        {
            bNeedsToCompileExpression = false;

            // copy expression from input and format
            std::string expression = std::string(TCHAR_TO_UTF8(**InputExpr));
            FExpressionOperatorPrivate::FormatExpr(expression);

            int err;
            // Compile the expression with variables.
            Expr = te_compile(expression.c_str(), Vars.data(), Vars.size(), &err);
            // Output error for failed compilation
            if (!Expr)
            {
                if constexpr (LOG_COMPILE_ERRORS)
                {
                    UE_LOG(LogTemp, Log, TEXT("Error: Invalid expression; error at char %d"), err);
                    UE_LOG(LogTemp, Log, TEXT("V1: %s"), *FString(Vars[0].name));
                }
                *OutputErrorMessage = "Invalid expression; error at char " + FString::FromInt(err);
                OutputErrorTrigger->TriggerFrame(0);
            }
        }

        // if successfully compiled expression
        if (Expr)
        {
            // compute
            FExpressionOperatorPrivate::TExpression<ValueType, NumArgs>::SampleExpression(Expr, X, DataInputs, DataOutput);
        }
#else
        *OutputFloat = *InputFloat;
#endif
    }

    template <typename ValueType, uint32 NumArgs>
    const FVertexInterface& FExpressionOperator<ValueType, NumArgs>::GetVertexInterface()
    {
        using namespace ExpressionNode;

        auto CreateInterface = []() -> FVertexInterface
        {
            FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertex<FString>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameExpr))
                ),

                FOutputVertexInterface(
                    TOutputDataVertex<ValueType>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameData)),
                    TOutputDataVertex<FTrigger>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameErrorTrigger)),
                    TOutputDataVertex<FString>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameErrorMessage))
                )
            );

            for (uint32 i = 0; i < NumArgs; ++i)
            {
                const FDataVertexMetadata InputDataMetadata
                {
                    GetInputDataDescription(i),
                    GetInputDataDisplayName(i)
                };

                Interface.GetInputInterface().Add(TInputDataVertex<ValueType>(GetInputDataName(i), InputDataMetadata));
            }

            return Interface;
        };

        static const FVertexInterface Interface = CreateInterface();
        return Interface;
    }

    template <typename ValueType, uint32 NumArgs>
    const FNodeClassMetadata& FExpressionOperator<ValueType, NumArgs>::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName = { TEXT("UE"), *FString::Printf(TEXT("Expression (%d)"), NumArgs), GetMetasoundDataTypeName<ValueType>() };
            Info.MajorVersion = 1;
            Info.MinorVersion = 0;
            Info.DisplayName = METASOUND_LOCTEXT_FORMAT("Metasound_ExpressionDisplayName", "Expression ({0}, {1})", GetMetasoundDataTypeDisplayText<ValueType>(), NumArgs);
            Info.Description = ExpressionNode::GetNodeDescription(NumArgs);
            Info.Author = PluginAuthor;
            Info.PromptIfMissing = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_ExpressionNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    template <typename ValueType, uint32 NumArgs>
    TUniquePtr<IOperator> FExpressionOperator<ValueType, NumArgs>::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace ExpressionNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface = GetVertexInterface().GetInputInterface();

        FStringReadRef ExprIn = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FString>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameExpr), InParams.OperatorSettings);
        TArray<TDataReadReference<ValueType>> DataIns;

        for (uint32 i = 0; i < NumArgs; ++i)
        {
            DataIns.Add(FExpressionOperatorPrivate::TExpression<ValueType, NumArgs>::CreateInRefData(InParams, i));
        }

        return MakeUnique<FExpressionOperator>(InParams.OperatorSettings, ExprIn, MoveTemp(DataIns));
    }


    //------------------------------------------------------------------------------------
    // FExpressionNode
    //------------------------------------------------------------------------------------
    template <typename ValueType, uint32 NumArgs>
    FExpressionNode<ValueType, NumArgs>::FExpressionNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FExpressionOperator<ValueType, NumArgs>>())
    {

    }

    REGISTER_EXPRESSION_NODE(float, 0)
    REGISTER_EXPRESSION_NODE(float, 1)
    REGISTER_EXPRESSION_NODE(float, 2)
    REGISTER_EXPRESSION_NODE(float, 3)
    REGISTER_EXPRESSION_NODE(float, 4)
    REGISTER_EXPRESSION_NODE(float, 5)
    REGISTER_EXPRESSION_NODE(float, 6)
    REGISTER_EXPRESSION_NODE(float, 7)
    REGISTER_EXPRESSION_NODE(float, 8)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 0)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 1)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 2)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 3)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 4)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 5)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 6)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 7)
    REGISTER_EXPRESSION_NODE(FAudioBuffer, 8)
}

#undef LOCTEXT_NAMESPACE
