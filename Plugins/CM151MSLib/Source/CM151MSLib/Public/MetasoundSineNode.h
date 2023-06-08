#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_Sine"

    namespace SineNode
    {
        METASOUND_PARAM(InParamNameIn, "In", "Input, multiplied by 2PI.")
        METASOUND_PARAM(OutParamNameOut, "Out", "Output, sin(in * 2PI).")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FSineOperator
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    class FSineOperator : public TExecutableOperator<FSineOperator<ValueType>>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FSineOperator(const FOperatorSettings& InSettings, const TDataReadReference<ValueType>& InInInput);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        TDataReadReference<ValueType>  InInput;
        TDataWriteReference<ValueType> OutOutput;
    };

    //------------------------------------------------------------------------------------
    // FSineNode
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    class FSineNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FSineNode(const FNodeInitData& InitData);
    };
}
