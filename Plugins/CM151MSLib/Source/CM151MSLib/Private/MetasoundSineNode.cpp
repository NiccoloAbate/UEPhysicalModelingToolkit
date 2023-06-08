#include "MetasoundSineNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SampleSineNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FSamnpleSineOperator
    //------------------------------------------------------------------------------------
    FSamnpleSineOperator::FSamnpleSineOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput)
        : AudioInput(InAudioInput)
        , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
    {
    }

    FDataReferenceCollection FSamnpleSineOperator::GetInputs() const
    {
        using namespace SampleSineNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);

        return InputDataReferences;
    }

    FDataReferenceCollection FSamnpleSineOperator::GetOutputs() const
    {
        using namespace SampleSineNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);

        return OutputDataReferences;
    }

    void FSamnpleSineOperator::Execute()
    {
        const float* InputAudio = AudioInput->GetData();
        float* OutputAudio = AudioOutput->GetData();

        const int NumSamples = AudioInput->Num();
        for (int i = 0; i < NumSamples; ++i)
            OutputAudio[i] = FGenericPlatformMath::Sin(InputAudio[i] * 2.0f * PI);
    }

    const FVertexInterface& FSamnpleSineOperator::GetVertexInterface()
    {
        using namespace SampleSineNode;

        static const FVertexInterface Interface(
            FInputVertexInterface(
                TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput))
            ),

            FOutputVertexInterface(
                TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameAudio))
            )
        );

        return Interface;
    }

    const FNodeClassMetadata& FSamnpleSineOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName = { TEXT("UE"), TEXT("SampleSine"), TEXT("Audio") };
            Info.MajorVersion = 1;
            Info.MinorVersion = 0;
            Info.DisplayName = LOCTEXT("Metasound_SampleSineDisplayName", "Sample Sine");
            Info.Description = LOCTEXT("Metasound_SampleSineNodeDescription", "Samples Sine function for each input sample, where 0-1 is one period.");
            Info.Author = PluginAuthor;
            Info.PromptIfMissing = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_SampleSineNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FSamnpleSineOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace SampleSineNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface = GetVertexInterface().GetInputInterface();

        FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);

        return MakeUnique<FSamnpleSineOperator>(InParams.OperatorSettings, AudioIn);
    }


    //------------------------------------------------------------------------------------
    // FSamnpleSineNode
    //------------------------------------------------------------------------------------
    FSamnpleSineNode::FSamnpleSineNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSamnpleSineOperator>())
    {

    }

    METASOUND_REGISTER_NODE(FSamnpleSineNode)
}

#undef LOCTEXT_NAMESPACE
