// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"
#include "Runtime/Engine/Classes/Engine/GameInstance.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeMultiplayerSessionsSubsystem() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_UGameInstanceSubsystem();
MULTIPLAYERSESSIONS_API UClass* Z_Construct_UClass_UMultiplayerSessionsSubsystem();
MULTIPLAYERSESSIONS_API UClass* Z_Construct_UClass_UMultiplayerSessionsSubsystem_NoRegister();
MULTIPLAYERSESSIONS_API UFunction* Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature();
MULTIPLAYERSESSIONS_API UFunction* Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature();
MULTIPLAYERSESSIONS_API UFunction* Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature();
UPackage* Z_Construct_UPackage__Script_MultiplayerSessions();
// End Cross Module References

// Begin Delegate FMultiplayerOnCreateSessionComplete
struct Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics
{
	struct _Script_MultiplayerSessions_eventMultiplayerOnCreateSessionComplete_Parms
	{
		bool wasSuccsessful;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "//Declaring our own custom delegates to bind callbacks to\n" },
#endif
		{ "ModuleRelativePath", "Public/MultiplayerSessionsSubsystem.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Declaring our own custom delegates to bind callbacks to" },
#endif
	};
#endif // WITH_METADATA
	static void NewProp_wasSuccsessful_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_wasSuccsessful;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
void Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful_SetBit(void* Obj)
{
	((_Script_MultiplayerSessions_eventMultiplayerOnCreateSessionComplete_Parms*)Obj)->wasSuccsessful = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful = { "wasSuccsessful", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(_Script_MultiplayerSessions_eventMultiplayerOnCreateSessionComplete_Parms), &Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::FuncParams = { (UObject*(*)())Z_Construct_UPackage__Script_MultiplayerSessions, nullptr, "MultiplayerOnCreateSessionComplete__DelegateSignature", nullptr, nullptr, Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::PropPointers), sizeof(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::_Script_MultiplayerSessions_eventMultiplayerOnCreateSessionComplete_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::Function_MetaDataParams), Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::_Script_MultiplayerSessions_eventMultiplayerOnCreateSessionComplete_Parms) < MAX_uint16);
UFunction* Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnCreateSessionComplete__DelegateSignature_Statics::FuncParams);
	}
	return ReturnFunction;
}
void FMultiplayerOnCreateSessionComplete_DelegateWrapper(const FMulticastScriptDelegate& MultiplayerOnCreateSessionComplete, bool wasSuccsessful)
{
	struct _Script_MultiplayerSessions_eventMultiplayerOnCreateSessionComplete_Parms
	{
		bool wasSuccsessful;
	};
	_Script_MultiplayerSessions_eventMultiplayerOnCreateSessionComplete_Parms Parms;
	Parms.wasSuccsessful=wasSuccsessful ? true : false;
	MultiplayerOnCreateSessionComplete.ProcessMulticastDelegate<UObject>(&Parms);
}
// End Delegate FMultiplayerOnCreateSessionComplete

// Begin Delegate FMultiplayerOnDestroySessionComplete
struct Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics
{
	struct _Script_MultiplayerSessions_eventMultiplayerOnDestroySessionComplete_Parms
	{
		bool wasSuccsessful;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/MultiplayerSessionsSubsystem.h" },
	};
#endif // WITH_METADATA
	static void NewProp_wasSuccsessful_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_wasSuccsessful;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
void Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful_SetBit(void* Obj)
{
	((_Script_MultiplayerSessions_eventMultiplayerOnDestroySessionComplete_Parms*)Obj)->wasSuccsessful = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful = { "wasSuccsessful", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(_Script_MultiplayerSessions_eventMultiplayerOnDestroySessionComplete_Parms), &Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::FuncParams = { (UObject*(*)())Z_Construct_UPackage__Script_MultiplayerSessions, nullptr, "MultiplayerOnDestroySessionComplete__DelegateSignature", nullptr, nullptr, Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::PropPointers), sizeof(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::_Script_MultiplayerSessions_eventMultiplayerOnDestroySessionComplete_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::Function_MetaDataParams), Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::_Script_MultiplayerSessions_eventMultiplayerOnDestroySessionComplete_Parms) < MAX_uint16);
UFunction* Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnDestroySessionComplete__DelegateSignature_Statics::FuncParams);
	}
	return ReturnFunction;
}
void FMultiplayerOnDestroySessionComplete_DelegateWrapper(const FMulticastScriptDelegate& MultiplayerOnDestroySessionComplete, bool wasSuccsessful)
{
	struct _Script_MultiplayerSessions_eventMultiplayerOnDestroySessionComplete_Parms
	{
		bool wasSuccsessful;
	};
	_Script_MultiplayerSessions_eventMultiplayerOnDestroySessionComplete_Parms Parms;
	Parms.wasSuccsessful=wasSuccsessful ? true : false;
	MultiplayerOnDestroySessionComplete.ProcessMulticastDelegate<UObject>(&Parms);
}
// End Delegate FMultiplayerOnDestroySessionComplete

// Begin Delegate FMultiplayerOnStartSessionComplete
struct Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics
{
	struct _Script_MultiplayerSessions_eventMultiplayerOnStartSessionComplete_Parms
	{
		bool wasSuccsessful;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/MultiplayerSessionsSubsystem.h" },
	};
#endif // WITH_METADATA
	static void NewProp_wasSuccsessful_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_wasSuccsessful;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
void Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful_SetBit(void* Obj)
{
	((_Script_MultiplayerSessions_eventMultiplayerOnStartSessionComplete_Parms*)Obj)->wasSuccsessful = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful = { "wasSuccsessful", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(_Script_MultiplayerSessions_eventMultiplayerOnStartSessionComplete_Parms), &Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::NewProp_wasSuccsessful,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::FuncParams = { (UObject*(*)())Z_Construct_UPackage__Script_MultiplayerSessions, nullptr, "MultiplayerOnStartSessionComplete__DelegateSignature", nullptr, nullptr, Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::PropPointers), sizeof(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::_Script_MultiplayerSessions_eventMultiplayerOnStartSessionComplete_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::Function_MetaDataParams), Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::_Script_MultiplayerSessions_eventMultiplayerOnStartSessionComplete_Parms) < MAX_uint16);
UFunction* Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UDelegateFunction_MultiplayerSessions_MultiplayerOnStartSessionComplete__DelegateSignature_Statics::FuncParams);
	}
	return ReturnFunction;
}
void FMultiplayerOnStartSessionComplete_DelegateWrapper(const FMulticastScriptDelegate& MultiplayerOnStartSessionComplete, bool wasSuccsessful)
{
	struct _Script_MultiplayerSessions_eventMultiplayerOnStartSessionComplete_Parms
	{
		bool wasSuccsessful;
	};
	_Script_MultiplayerSessions_eventMultiplayerOnStartSessionComplete_Parms Parms;
	Parms.wasSuccsessful=wasSuccsessful ? true : false;
	MultiplayerOnStartSessionComplete.ProcessMulticastDelegate<UObject>(&Parms);
}
// End Delegate FMultiplayerOnStartSessionComplete

// Begin Class UMultiplayerSessionsSubsystem
void UMultiplayerSessionsSubsystem::StaticRegisterNativesUMultiplayerSessionsSubsystem()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UMultiplayerSessionsSubsystem);
UClass* Z_Construct_UClass_UMultiplayerSessionsSubsystem_NoRegister()
{
	return UMultiplayerSessionsSubsystem::StaticClass();
}
struct Z_Construct_UClass_UMultiplayerSessionsSubsystem_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "IncludePath", "MultiplayerSessionsSubsystem.h" },
		{ "ModuleRelativePath", "Public/MultiplayerSessionsSubsystem.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMultiplayerSessionsSubsystem>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UMultiplayerSessionsSubsystem_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UGameInstanceSubsystem,
	(UObject* (*)())Z_Construct_UPackage__Script_MultiplayerSessions,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMultiplayerSessionsSubsystem_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMultiplayerSessionsSubsystem_Statics::ClassParams = {
	&UMultiplayerSessionsSubsystem::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMultiplayerSessionsSubsystem_Statics::Class_MetaDataParams), Z_Construct_UClass_UMultiplayerSessionsSubsystem_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UMultiplayerSessionsSubsystem()
{
	if (!Z_Registration_Info_UClass_UMultiplayerSessionsSubsystem.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMultiplayerSessionsSubsystem.OuterSingleton, Z_Construct_UClass_UMultiplayerSessionsSubsystem_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMultiplayerSessionsSubsystem.OuterSingleton;
}
template<> MULTIPLAYERSESSIONS_API UClass* StaticClass<UMultiplayerSessionsSubsystem>()
{
	return UMultiplayerSessionsSubsystem::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UMultiplayerSessionsSubsystem);
UMultiplayerSessionsSubsystem::~UMultiplayerSessionsSubsystem() {}
// End Class UMultiplayerSessionsSubsystem

// Begin Registration
struct Z_CompiledInDeferFile_FID_UnrealEngine_Projects_MultiplayerShooter___Copy_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_MultiplayerSessionsSubsystem_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMultiplayerSessionsSubsystem, UMultiplayerSessionsSubsystem::StaticClass, TEXT("UMultiplayerSessionsSubsystem"), &Z_Registration_Info_UClass_UMultiplayerSessionsSubsystem, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMultiplayerSessionsSubsystem), 2032748146U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_UnrealEngine_Projects_MultiplayerShooter___Copy_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_MultiplayerSessionsSubsystem_h_3839795320(TEXT("/Script/MultiplayerSessions"),
	Z_CompiledInDeferFile_FID_UnrealEngine_Projects_MultiplayerShooter___Copy_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_MultiplayerSessionsSubsystem_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_UnrealEngine_Projects_MultiplayerShooter___Copy_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_MultiplayerSessionsSubsystem_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
