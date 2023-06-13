#include "MetasoundSineNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SineNode"

namespace Metasound
{
    namespace FSineOperatorPrivate
    {
        template <typename ValueType>
        struct TSine
        {
            bool bIsSupported = false;
        };

        template <>
        struct TSine<float>
        {
            static void Sine(float In, float& Out)
            {
                Out = FGenericPlatformMath::Sin(In * 2.0f * PI);
            }

            static TDataReadReference<float> CreateInRefData(const FCreateOperatorParams& InParams)
            {
                using namespace SineNode;

                const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();

                return InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameIn), InParams.OperatorSettings);
            }
        };

        template <>
        struct TSine<FAudioBuffer>
        {
            static void Sine(const FAudioBuffer& In, FAudioBuffer& Out)
            {
                const float* InAudio = In.GetData();
                float* OutAudio = Out.GetData();

                const int NumSamples = In.Num();
                for (int i = 0; i < NumSamples; ++i)
                    OutAudio[i] = FGenericPlatformMath::Sin(InAudio[i] * 2.0f * PI);
            }

            static TDataReadReference<FAudioBuffer> CreateInRefData(const FCreateOperatorParams& InParams)
            {
                using namespace SineNode;

                const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
                const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();

                return InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameIn), InParams.OperatorSettings);
            }
        };
    }

    //------------------------------------------------------------------------------------
    // FSineOperator
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    FSineOperator<ValueType>::FSineOperator(const FOperatorSettings& InSettings, const TDataReadReference<ValueType>& InInInput)
        : InInput(InInInput)
        , OutOutput(TDataWriteReferenceFactory<ValueType>::CreateAny(InSettings))
    {
    }

    template <typename ValueType>
    FDataReferenceCollection FSineOperator<ValueType>::GetInputs() const
    {
        using namespace SineNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameIn), InInput);

        return InputDataReferences;
    }

    template <typename ValueType>
    FDataReferenceCollection FSineOperator<ValueType>::GetOutputs() const
    {
        using namespace SineNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameOut), OutOutput);

        return OutputDataReferences;
    }

    template <typename ValueType>
    void FSineOperator<ValueType>::Execute()
    {
        FSineOperatorPrivate::TSine<ValueType>::Sine(*InInput, *OutOutput);
    }

    template <typename ValueType>
    const FVertexInterface& FSineOperator<ValueType>::GetVertexInterface()
    {
        using namespace SineNode;

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
    const FNodeClassMetadata& FSineOperator<ValueType>::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName = { TEXT("UE"), TEXT("Sine"), GetMetasoundDataTypeName<ValueType>() };
            Info.MajorVersion = 1;
            Info.MinorVersion = 0;
            Info.DisplayName = METASOUND_LOCTEXT_FORMAT("Metasound_CosineDisplayName", "Sin ({0})", GetMetasoundDataTypeDisplayText<ValueType>());
            Info.Description = LOCTEXT("Metasound_SineNodeDescription", "Samples Sine function for each input, where 0-1 is one period.");
            Info.Author = PluginAuthor;
            Info.PromptIfMissing = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_SineNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    template <typename ValueType>
    TUniquePtr<IOperator> FSineOperator<ValueType>::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace SineNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface = GetVertexInterface().GetInputInterface();

        TDataReadReference<ValueType> InIn = FSineOperatorPrivate::TSine<ValueType>::CreateInRefData(InParams);

        return MakeUnique<FSineOperator<ValueType>>(InParams.OperatorSettings, InIn);
    }


    //------------------------------------------------------------------------------------
    // FSineNode
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    FSineNode<ValueType>::FSineNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSineOperator<ValueType>>())
    {

    }

    using FSineNodeFloat = FSineNode<float>;
    METASOUND_REGISTER_NODE(FSineNodeFloat)

    using FCosineNodeAudio = FSineNode<FAudioBuffer>;
    METASOUND_REGISTER_NODE(FCosineNodeAudio)

    template class FSineNode<float>;
    template class FSineNode<FAudioBuffer>;
}

#undef LOCTEXT_NAMESPACE
