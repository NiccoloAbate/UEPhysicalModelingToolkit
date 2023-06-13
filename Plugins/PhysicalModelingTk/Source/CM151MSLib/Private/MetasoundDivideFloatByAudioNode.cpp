#include "MetasoundDivideFloatByAudioNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_DivideFloatByAudioNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FDivideFloatByAudioOperator
    //------------------------------------------------------------------------------------
    FDivideFloatByAudioOperator::FDivideFloatByAudioOperator(const FOperatorSettings& InSettings, const FFloatReadRef& InFloatInput, const FAudioBufferReadRef& InAudioInput)
        : FloatInput(InFloatInput)
        , AudioInput(InAudioInput)
        , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
    {
    }

    FDataReferenceCollection FDivideFloatByAudioOperator::GetInputs() const
    {
        using namespace DivideFloatByAudioNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameFloatInput), FloatInput);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);

        return InputDataReferences;
    }

    FDataReferenceCollection FDivideFloatByAudioOperator::GetOutputs() const
    {
        using namespace DivideFloatByAudioNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);

        return OutputDataReferences;
    }

    void FDivideFloatByAudioOperator::Execute()
    {
        const float* InputFloat = FloatInput.Get();
        const float* InputAudio = AudioInput->GetData();
        float* OutputAudio = AudioOutput->GetData();

        constexpr float NOTZERO = 0.000000001f;

        const int32 NumSamples = AudioInput->Num();
        for (int i = 0; i < NumSamples; ++i)
            OutputAudio[i] = *InputFloat / (InputAudio[i] != 0.0f ? InputAudio[i] : NOTZERO);
    }

    const FVertexInterface& FDivideFloatByAudioOperator::GetVertexInterface()
    {
        using namespace DivideFloatByAudioNode;

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

    const FNodeClassMetadata& FDivideFloatByAudioOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName        = { TEXT("UE"), TEXT("DivideFloatByAudio"), TEXT("Audio") };
            Info.MajorVersion     = 1;
            Info.MinorVersion     = 0;
            Info.DisplayName      = LOCTEXT("Metasound_DivideFloatByAudioDisplayName", "Divide (Float By Audio)");
            Info.Description      = LOCTEXT("Metasound_DivideFloatByAudioNodeDescription", "Divides float by audio.");
            Info.Author           = PluginAuthor;
            Info.PromptIfMissing  = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_DivideFloatByAudioNodeCategory", "Math") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FDivideFloatByAudioOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace DivideFloatByAudioNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

        FFloatReadRef FloatIn = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameFloatInput), InParams.OperatorSettings);
        FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);

        return MakeUnique<FDivideFloatByAudioOperator>(InParams.OperatorSettings, FloatIn, AudioIn);
    }


    //------------------------------------------------------------------------------------
    // FDivideFloatByAudioNode
    //------------------------------------------------------------------------------------
    FDivideFloatByAudioNode::FDivideFloatByAudioNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FDivideFloatByAudioOperator>())
    {
    
    }

    METASOUND_REGISTER_NODE(FDivideFloatByAudioNode)
}

#undef LOCTEXT_NAMESPACE
