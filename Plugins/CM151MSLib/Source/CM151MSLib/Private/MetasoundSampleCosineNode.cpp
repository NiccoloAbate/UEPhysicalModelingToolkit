#include "MetasoundSampleCosineNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SampleCosineNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FSamnpleCosineOperator
    //------------------------------------------------------------------------------------
    FSamnpleCosineOperator::FSamnpleCosineOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput)
        : AudioInput(InAudioInput)
        , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
    {
    }

    FDataReferenceCollection FSamnpleCosineOperator::GetInputs() const
    {
        using namespace SampleCosineNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);

        return InputDataReferences;
    }

    FDataReferenceCollection FSamnpleCosineOperator::GetOutputs() const
    {
        using namespace SampleCosineNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);

        return OutputDataReferences;
    }

    void FSamnpleCosineOperator::Execute()
    {
        const float* InputAudio = AudioInput->GetData();
        float* OutputAudio = AudioOutput->GetData();

        const int NumSamples = AudioInput->Num();
        for (int i = 0; i < NumSamples; ++i)
            OutputAudio[i] = FGenericPlatformMath::Cos(InputAudio[i] * 2.0f *PI);
    }

    const FVertexInterface& FSamnpleCosineOperator::GetVertexInterface()
    {
        using namespace SampleCosineNode;

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

    const FNodeClassMetadata& FSamnpleCosineOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName        = { TEXT("UE"), TEXT("SampleCosine"), TEXT("Audio") };
            Info.MajorVersion     = 1;
            Info.MinorVersion     = 0;
            Info.DisplayName      = LOCTEXT("Metasound_SampleCosineDisplayName", "Sample Cosine");
            Info.Description      = LOCTEXT("Metasound_SampleCosineNodeDescription", "Samples Cosine function for each input sample, where 0-1 is one period.");
            Info.Author           = PluginAuthor;
            Info.PromptIfMissing  = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_SampleCosineNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FSamnpleCosineOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace SampleCosineNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

        FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);

        return MakeUnique<FSamnpleCosineOperator>(InParams.OperatorSettings, AudioIn);
    }


    //------------------------------------------------------------------------------------
    // FSamnpleCosineNode
    //------------------------------------------------------------------------------------
    FSamnpleCosineNode::FSamnpleCosineNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSamnpleCosineOperator>())
    {
    
    }

    METASOUND_REGISTER_NODE(FSamnpleCosineNode)
}

#undef LOCTEXT_NAMESPACE
