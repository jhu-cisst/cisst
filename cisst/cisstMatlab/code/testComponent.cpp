#include "testComponent.h"

#include <cisstVector/vctRandom.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

CMN_IMPLEMENT_SERVICES(testComponent);

testComponent::testComponent(const std::string & name):
    mtsTaskPeriodic(name, 10.0 * cmn_ms, false, 500),
    Counter(0)
{
    Data.SetSize(5);
    vctRandom(Data, 0.0, 10.0);
    
    StateTable.AddData(this->Data, "Data");

    mtsInterfaceProvided * interfaceProvided;

    interfaceProvided = this->AddInterfaceProvided("interface1");
    interfaceProvided->AddCommandVoid(&testComponent::Zero, this, "Zero");
    interfaceProvided->AddCommandVoid(&testComponent::Hundred, this, "Hundred");

    interfaceProvided = this->AddInterfaceProvided("interface2");
    interfaceProvided->AddCommandWrite(&testComponent::Set, this, "Set");
    interfaceProvided->AddCommandReadState(this->StateTable, this->Data, "Get");
}

  
void testComponent::Startup(void)
{
}


void testComponent::Run(void)
{
    ProcessQueuedCommands();
    Counter++;
    if ((Counter % 100) == 0) {
	Data.Add(0.01);
	CMN_LOG_CLASS_RUN_ERROR << "Counter: " << Counter << std::endl
				<< Data << std::endl;

    }
}


CMN_IMPLEMENT_SERVICES(testComponentProxy);

testComponentProxy::testComponentProxy(const std::string & name):
    mtsComponent(name)
{
    mtsInterfaceRequired * interfaceRequired;

    interfaceRequired = this->AddInterfaceRequired("interface1");
    interfaceRequired->AddFunction("Zero", this->Zero);
    interfaceRequired->AddFunction("Hundred", this->Hundred);

    interfaceRequired = this->AddInterfaceRequired("interface2");
    interfaceRequired->AddFunction("Set", this->Set);
    interfaceRequired->AddFunction("Get", this->Get);
}
