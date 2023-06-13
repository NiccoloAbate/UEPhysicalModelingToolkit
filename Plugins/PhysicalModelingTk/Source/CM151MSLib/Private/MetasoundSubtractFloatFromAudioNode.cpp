#include "MetasoundSubtractFloatFromAudioNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SubtractFloatFromAudioNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FSubtractFloatFromAudioOperator
    //------------------------------------------------------------------------------------
    FSubtractFloatFromAudioOperator::FSubtractFloatFromAudioOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FFloatReadRef& InFloatInput)
        : AudioInput(InAudioInput)
        , FloatInput(InFloatInput)
        , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
    {
    }

    FDataReferenceCollection FSubtractFloatFromAudioOperator::GetInputs() const
    {
        using namespace SubtractFloatFromNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameFloatInput), FloatInput);

        return InputDataReferences;
    }

    FDataReferenceCollection FSubtractFloatFromAudioOperator::GetOutputs() const
    {
        using namespace SubtractFloatFromNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);

        return OutputDataReferences;
    }

    void FSubtractFloatFromAudioOperator::Execute()
    {
        const float* InputAudio = AudioInput->GetData();
        const float* InputFloat = FloatInput.Get();
        float* OutputAudio = AudioOutput->GetData();

        const int32 NumSamples = AudioInput->Num();
        for (int i = 0; i < NumSamples; ++i)
            OutputAudio[i] = InputAudio[i] - *InputFloat;
    }

    const FVertexInterface& FSubtractFloatFromAudioOperator::GetVertexInterface()
    {
        using namespace SubtractFloatFromNode;

        static const FVertexInterface Interface(
            FInputVertexInterface(
                TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput)),
                TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameFloatInput), 1.0f)
            ),

            FOutputVertexInterface(
                TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameAudio))
            )
        );

        return Interface;
    }

    const FNodeClassMetadata& FSubtractFloatFromAudioOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName        = { TEXT("UE"), TEXT("SubtractFloatFromAudio"), TEXT("Audio") };
            Info.MajorVersion     = 1;
            Info.MinorVersion     = 0;
            Info.DisplayName      = LOCTEXT("Metasound_SubtractFloatFromAudioDisplayName", "Subtract (Float From Audio)");
            Info.Description      = LOCTEXT("Metasound_SubtractFloatFromAudioNodeDescription", "Subtracts float from audio.");
            Info.Author           = PluginAuthor;
            Info.PromptIfMissing  = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_SubtractFloatFromAudioNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FSubtractFloatFromAudioOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace SubtractFloatFromNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

        FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);
        FFloatReadRef FloatIn = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameFloatInput), InParams.OperatorSettings);

        return MakeUnique<FSubtractFloatFromAudioOperator>(InParams.OperatorSettings, AudioIn, FloatIn);
    }


    //------------------------------------------------------------------------------------
    // FSubtractFloatFromAudioNode
    //------------------------------------------------------------------------------------
    FSubtractFloatFromAudioNode::FSubtractFloatFromAudioNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSubtractFloatFromAudioOperator>())
    {
    
    }

    METASOUND_REGISTER_NODE(FSubtractFloatFromAudioNode)
}

#undef LOCTEXT_NAMESPACE
