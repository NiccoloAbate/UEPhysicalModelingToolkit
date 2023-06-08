#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_Cosine"

    namespace CosineNode
    {
        METASOUND_PARAM(InParamNameIn, "In",        "Input, multiplied by 2PI.")
        METASOUND_PARAM(OutParamNameOut,     "Out",       "Output, cos(in * 2PI).")
    }

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FCosineOperator
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    class FCosineOperator : public TExecutableOperator<FCosineOperator<ValueType>>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FCosineOperator(const FOperatorSettings& InSettings, const TDataReadReference<ValueType>& InInInput);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        TDataReadReference<ValueType>  InInput;
        TDataWriteReference<ValueType> OutOutput;
    };

    //------------------------------------------------------------------------------------
    // FCosineNode
    //------------------------------------------------------------------------------------
    template <typename ValueType>
    class FCosineNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FCosineNode(const FNodeInitData& InitData);
    };
}
