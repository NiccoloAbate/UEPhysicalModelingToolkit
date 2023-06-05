#include "MetasoundSubtractAudioFromFloatNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_DivideFloatByAudioNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FSubtractAudioFromFloatOperator
    //------------------------------------------------------------------------------------
    FSubtractAudioFromFloatOperator::FSubtractAudioFromFloatOperator(const FOperatorSettings& InSettings, const FFloatReadRef& InFloatInput, const FAudioBufferReadRef& InAudioInput)
        : FloatInput(InFloatInput)
        , AudioInput(InAudioInput)
        , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
    {
    }

    FDataReferenceCollection FSubtractAudioFromFloatOperator::GetInputs() const
    {
        using namespace SubtractAudioFromFloatNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameFloatInput), FloatInput);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);

        return InputDataReferences;
    }

    FDataReferenceCollection FSubtractAudioFromFloatOperator::GetOutputs() const
    {
        using namespace SubtractAudioFromFloatNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);

        return OutputDataReferences;
    }

    void FSubtractAudioFromFloatOperator::Execute()
    {
        const float* InputFloat = FloatInput.Get();
        const float* InputAudio = AudioInput->GetData();
        float* OutputAudio = AudioOutput->GetData();

        const int32 NumSamples = AudioInput->Num();
        for (int i = 0; i < NumSamples; ++i)
            OutputAudio[i] = InputAudio[i] - *InputFloat;
    }

    const FVertexInterface& FSubtractAudioFromFloatOperator::GetVertexInterface()
    {
        using namespace SubtractAudioFromFloatNode;

        static const FVertexInterface Interface(
            FInputVertexInterface(
                TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameFloatInput), 1.0f),
                TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput))
            ),

            FOutputVertexInterface(
                TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameAudio))
            )
        );

        return Interface;
    }

    const FNodeClassMetadata& FSubtractAudioFromFloatOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName        = { TEXT("UE"), TEXT("SubtractAudioFromFloat"), TEXT("Audio") };
            Info.MajorVersion     = 1;
            Info.MinorVersion     = 0;
            Info.DisplayName      = LOCTEXT("Metasound_SubtractAudioFromFloatDisplayName", "Subtract (Audio From Float)");
            Info.Description      = LOCTEXT("Metasound_SubtractAudioFromFloatNodeDescription", "Subtracts audio from float.");
            Info.Author           = PluginAuthor;
            Info.PromptIfMissing  = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_SubtractAudioFromFloatNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FSubtractAudioFromFloatOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace SubtractAudioFromFloatNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

        FFloatReadRef FloatIn = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameFloatInput), InParams.OperatorSettings);
        FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);

        return MakeUnique<FSubtractAudioFromFloatOperator>(InParams.OperatorSettings, FloatIn, AudioIn);
    }


    //------------------------------------------------------------------------------------
    // FSubtractAudioFromFloatNode
    //------------------------------------------------------------------------------------
    FSubtractAudioFromFloatNode::FSubtractAudioFromFloatNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSubtractAudioFromFloatOperator>())
    {
    
    }

    METASOUND_REGISTER_NODE(FSubtractAudioFromFloatNode)
}

#undef LOCTEXT_NAMESPACE
