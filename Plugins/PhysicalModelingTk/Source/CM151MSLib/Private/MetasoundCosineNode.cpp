#include "MetasoundCosineNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_CosineNode"

namespace Metasound
{
    namespace FCosineOperatorPrivate
    {
        template <typename ValueType>
        struct TCosine
        {
            bool bIsSupported = false;
        };

        template <>
        struct TCosine<float>
        {
            static void Cosine(float In, float& Out)
            {
                Out = FGenericPlatformMath::Cos(In * 2.0f * PI);
            }

            static TDataReadReference<float> CreateInRefData(const FCreateOperatorParams& InParams)
            {
                using namespace CosineNode;

                const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();

                return InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameIn), InParams.OperatorSettings);
            }
        };

        template <>
        struct TCosine<FAudioBuffer>
        {
            static void Cosine(const FAudioBuffer& In, FAudioBuffer& Out)
            {
                const float* InAudio = In.GetData();
                float* OutAudio = Out.GetData();

                const int NumSamples = In.Num();
                for (int i = 0; i < NumSamples; ++i)
                    OutAudio[i] = FGenericPlatformMath::Cos(InAudio[i] * 2.0f * PI);
            }

            static TDataReadReference<FAudioBuffer> CreateInRefData(const FCreateOperatorParams& InParams)
            {
                using namespace CosineNode;

                const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();

                return InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameIn), InParams.OperatorSettings);
            }
        };
    }

    //------------------------------------------------------------------------------------
    // FCosineOperator
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    FCosineOperator<ValueType>::FCosineOperator(const FOperatorSettings& InSettings, const TDataReadReference<ValueType>& InInInput)
        : InInput(InInInput)
        , OutOutput(TDataWriteReferenceFactory<ValueType>::CreateAny(InSettings))
    {
    }

    template <typename ValueType>
    FDataReferenceCollection FCosineOperator<ValueType>::GetInputs() const
    {
        using namespace CosineNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameIn), InInput);

        return InputDataReferences;
    }

    template <typename ValueType>
    FDataReferenceCollection FCosineOperator<ValueType>::GetOutputs() const
    {
        using namespace CosineNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameOut), OutOutput);

        return OutputDataReferences;
    }

    template <typename ValueType>
    void FCosineOperator<ValueType>::Execute()
    {
        FCosineOperatorPrivate::TCosine<ValueType>::Cosine(*InInput, *OutOutput);
    }

    template <typename ValueType>
    const FVertexInterface& FCosineOperator<ValueType>::GetVertexInterface()
    {
        using namespace CosineNode;

        static const FVertexInterface Interface(
            FInputVertexInterface(
                TInputDataVertex<ValueType>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameIn))
            ),

            FOutputVertexInterface(
                TOutputDataVertex<ValueType>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameOut))
            )
        );

        return Interface;
    }

    template <typename ValueType>
    const FNodeClassMetadata& FCosineOperator<ValueType>::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName        = { TEXT("UE"), TEXT("Cosine"), GetMetasoundDataTypeName<ValueType>() };
            Info.MajorVersion     = 1;
            Info.MinorVersion     = 0;
            Info.DisplayName      = METASOUND_LOCTEXT_FORMAT("Metasound_CosineDisplayName", "Cos ({0})", GetMetasoundDataTypeDisplayText<ValueType>());
            Info.Description      = LOCTEXT("Metasound_CosineNodeDescription", "Samples Cosine function for each input, where 0-1 is one period.");
            Info.Author           = PluginAuthor;
            Info.PromptIfMissing  = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_CosineNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    template <typename ValueType>
    TUniquePtr<IOperator> FCosineOperator<ValueType>::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace CosineNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

        TDataReadReference<ValueType> InIn = FCosineOperatorPrivate::TCosine<ValueType>::CreateInRefData(InParams);

        return MakeUnique<FCosineOperator<ValueType>>(InParams.OperatorSettings, InIn);
    }


    //------------------------------------------------------------------------------------
    // FCosineNode
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    FCosineNode<ValueType>::FCosineNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FCosineOperator<ValueType>>())
    {
    
    }

    using FCosingNodeFloat = FCosineNode<float>;
    METASOUND_REGISTER_NODE(FCosingNodeFloat)

    using FCosingNodeAudio = FCosineNode<FAudioBuffer>;
    METASOUND_REGISTER_NODE(FCosingNodeAudio)

    template class FCosineNode<float>;
    template class FCosineNode<FAudioBuffer>;
}

#undef LOCTEXT_NAMESPACE
