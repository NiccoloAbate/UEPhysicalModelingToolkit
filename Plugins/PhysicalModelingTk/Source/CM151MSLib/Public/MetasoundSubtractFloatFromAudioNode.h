#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SubtractFloatFromAudio"

    namespace SubtractFloatFromNode
    {
        METASOUND_PARAM(InParamNameFloatInput, "In0",        "Float input.")
        METASOUND_PARAM(InParamNameAudioInput,  "In1", "Audio input.")
        METASOUND_PARAM(OutParamNameAudio,     "Out",       "Audio output.")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FSubtractFloatFromAudioOperator
    //------------------------------------------------------------------------------------
    class FSubtractFloatFromAudioOperator : public TExecutableOperator<FSubtractFloatFromAudioOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FSubtractFloatFromAudioOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FFloatReadRef& InFloatInput);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FAudioBufferReadRef  AudioInput;
        FFloatReadRef  FloatInput;
        FAudioBufferWriteRef AudioOutput;
    };

    //------------------------------------------------------------------------------------
    // FSubtractFloatFromAudioNode
    //------------------------------------------------------------------------------------
    class FSubtractFloatFromAudioNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FSubtractFloatFromAudioNode(const FNodeInitData& InitData);
    };
}
