#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SubtractAudioFromFloat"

    namespace SubtractAudioFromFloatNode
    {
        METASOUND_PARAM(InParamNameFloatInput, "In0",        "Float input.")
        METASOUND_PARAM(InParamNameAudioInput,  "In1", "Audio input.")
        METASOUND_PARAM(OutParamNameAudio,     "Out",       "Audio output.")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FSubtractAudioFromFloatOperator
    //------------------------------------------------------------------------------------
    class FSubtractAudioFromFloatOperator : public TExecutableOperator<FSubtractAudioFromFloatOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FSubtractAudioFromFloatOperator(const FOperatorSettings& InSettings, const FFloatReadRef& InFloatInput, const FAudioBufferReadRef& InAudioInput);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FFloatReadRef  FloatInput;
        FAudioBufferReadRef  AudioInput;
        FAudioBufferWriteRef AudioOutput;
    };

    //------------------------------------------------------------------------------------
    // FSubtractAudioFromFloatNode
    //------------------------------------------------------------------------------------
    class FSubtractAudioFromFloatNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FSubtractAudioFromFloatNode(const FNodeInitData& InitData);
    };
}
