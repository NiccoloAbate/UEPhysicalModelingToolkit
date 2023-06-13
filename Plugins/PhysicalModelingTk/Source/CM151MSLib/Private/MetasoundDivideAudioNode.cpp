#include "MetasoundDivideAudioNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_DivideAudioNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FVolumeOperator
    //------------------------------------------------------------------------------------
    FDivideAudioOperator::FDivideAudioOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput0, const FAudioBufferReadRef& InAudioInput1)
        : AudioInput0(InAudioInput0)
        , AudioInput1(InAudioInput1)
        , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
    {
    }

    FDataReferenceCollection FDivideAudioOperator::GetInputs() const
    {
        using namespace DivideAudioNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput0), AudioInput0);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput1), AudioInput1);

        return InputDataReferences;
    }

    FDataReferenceCollection FDivideAudioOperator::GetOutputs() const
    {
        using namespace DivideAudioNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);

        return OutputDataReferences;
    }

    void FDivideAudioOperator::Execute()
    {
        const float* InputAudio0 = AudioInput0->GetData();
        const float* InputAudio1 = AudioInput1->GetData();
        float* OutputAudio = AudioOutput->GetData();

        constexpr float NOTZERO = 0.000000001f;

        const int32 NumSamples = AudioInput0->Num();
        for (int i = 0; i < NumSamples; ++i)
            OutputAudio[i] = InputAudio0[i] / (InputAudio1[i] != 0.0f ? InputAudio1[i] : NOTZERO);
    }

    const FVertexInterface& FDivideAudioOperator::GetVertexInterface()
    {
        using namespace DivideAudioNode;

        static const FVertexInterface Interface(
            FInputVertexInterface(
                TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput0)),
                TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput1))
            ),

            FOutputVertexInterface(
                TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameAudio))
            )
        );

        return Interface;
    }

    const FNodeClassMetadata& FDivideAudioOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName        = { TEXT("UE"), TEXT("DivideAudio"), TEXT("Audio") };
            Info.MajorVersion     = 1;
            Info.MinorVersion     = 0;
            Info.DisplayName      = LOCTEXT("Metasound_DivideAudioDisplayName", "Divide (Audio)");
            Info.Description      = LOCTEXT("Metasound_DivideAudioNodeDescription", "Divides audio 1 by audio 2.");
            Info.Author           = PluginAuthor;
            Info.PromptIfMissing  = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_DivideAudioNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FDivideAudioOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace DivideAudioNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

        FAudioBufferReadRef AudioIn0 = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput0), InParams.OperatorSettings);
        FAudioBufferReadRef AudioIn1 = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput1), InParams.OperatorSettings);

        return MakeUnique<FDivideAudioOperator>(InParams.OperatorSettings, AudioIn0, AudioIn1);
    }


    //------------------------------------------------------------------------------------
    // FVolumeNode
    //------------------------------------------------------------------------------------
    FDivideAudioNode::FDivideAudioNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FDivideAudioOperator>())
    {
    
    }

    METASOUND_REGISTER_NODE(FDivideAudioNode)
}

#undef LOCTEXT_NAMESPACE
