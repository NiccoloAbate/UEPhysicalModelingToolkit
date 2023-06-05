#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SampleCosine"

    namespace SampleCosineNode
    {
        METASOUND_PARAM(InParamNameAudioInput, "In",        "Audio input.")
        METASOUND_PARAM(OutParamNameAudio,     "Out",       "Audio output.")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FSamnpleCosineOperator
    //------------------------------------------------------------------------------------
    class FSamnpleCosineOperator : public TExecutableOperator<FSamnpleCosineOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FSamnpleCosineOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FAudioBufferReadRef  AudioInput;
        FAudioBufferWriteRef AudioOutput;
    };

    //------------------------------------------------------------------------------------
    // FSamnpleCosineNode
    //------------------------------------------------------------------------------------
    class FSamnpleCosineNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FSamnpleCosineNode(const FNodeInitData& InitData);
    };
}
