#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SampleSine"

    namespace SampleSineNode
    {
        METASOUND_PARAM(InParamNameAudioInput, "In", "Audio input.")
        METASOUND_PARAM(OutParamNameAudio, "Out", "Audio output.")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FSamnpleSineOperator
    //------------------------------------------------------------------------------------
    class FSamnpleSineOperator : public TExecutableOperator<FSamnpleSineOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FSamnpleSineOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FAudioBufferReadRef  AudioInput;
        FAudioBufferWriteRef AudioOutput;
    };

    //------------------------------------------------------------------------------------
    // FSamnpleSineNode
    //------------------------------------------------------------------------------------
    class FSamnpleSineNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FSamnpleSineNode(const FNodeInitData& InitData);
    };
}
