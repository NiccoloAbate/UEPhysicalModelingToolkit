#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_DivideAudio"

    namespace DivideFloatByAudioNode
    {
        METASOUND_PARAM(InParamNameFloatInput, "In0",        "Float input (numerator).")
        METASOUND_PARAM(InParamNameAudioInput,  "In1", "Audio input (denominator).")
        METASOUND_PARAM(OutParamNameAudio,     "Out",       "Audio output.")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FDivideFloatByAudioOperator
    //------------------------------------------------------------------------------------
    class FDivideFloatByAudioOperator : public TExecutableOperator<FDivideFloatByAudioOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FDivideFloatByAudioOperator(const FOperatorSettings& InSettings, const FFloatReadRef& InFloatInput, const FAudioBufferReadRef& InAudioInput);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FFloatReadRef FloatInput;
        FAudioBufferReadRef  AudioInput;
        FAudioBufferWriteRef AudioOutput;
    };

    //------------------------------------------------------------------------------------
    // FDivideFloatByAudioNode
    //------------------------------------------------------------------------------------
    class FDivideFloatByAudioNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FDivideFloatByAudioNode(const FNodeInitData& InitData);
    };
}
