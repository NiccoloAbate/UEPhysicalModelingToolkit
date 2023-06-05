#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_DivideAudio"

    namespace DivideAudioNode
    {
        METASOUND_PARAM(InParamNameAudioInput0, "In0",        "Audio input 0 (numerator).")
        METASOUND_PARAM(InParamNameAudioInput1,  "In1", "Audio input 1 (denominator).")
        METASOUND_PARAM(OutParamNameAudio,     "Out",       "Audio output.")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FDivideAudioOperator
    //------------------------------------------------------------------------------------
    class FDivideAudioOperator : public TExecutableOperator<FDivideAudioOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FDivideAudioOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput0, const FAudioBufferReadRef& InAudioInput1);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FAudioBufferReadRef  AudioInput0;
        FAudioBufferReadRef  AudioInput1;
        FAudioBufferWriteRef AudioOutput;
    };

    //------------------------------------------------------------------------------------
    // FDivideAudioNode
    //------------------------------------------------------------------------------------
    class FDivideAudioNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FDivideAudioNode(const FNodeInitData& InitData);
    };
}
