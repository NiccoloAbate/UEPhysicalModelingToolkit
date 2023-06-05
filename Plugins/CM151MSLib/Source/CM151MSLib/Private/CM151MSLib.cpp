// Copyright Epic Games, Inc. All Rights Reserved.

#include "CM151MSLib.h"
#include "MetasoundFrontendRegistries.h"

#define LOCTEXT_NAMESPACE "FCM151MSLibModule"

void FCM151MSLibModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();
}

void FCM151MSLibModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCM151MSLibModule, CM151MSLib)