// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "MultiplayerSessions/Public/Menu.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeMenu() {}

// Begin Cross Module References
MULTIPLAYERSESSIONS_API UClass* Z_Construct_UClass_UMenu();
MULTIPLAYERSESSIONS_API UClass* Z_Construct_UClass_UMenu_NoRegister();
UMG_API UClass* Z_Construct_UClass_UButton_NoRegister();
UMG_API UClass* Z_Construct_UClass_UUserWidget();
UPackage* Z_Construct_UPackage__Script_MultiplayerSessions();
// End Cross Module References

// Begin Class UMenu Function HostButtonClicked
struct Z_Construct_UFunction_UMenu_HostButtonClicked_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Menu.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UMenu_HostButtonClicked_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UMenu, nullptr, "HostButtonClicked", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_HostButtonClicked_Statics::Function_MetaDataParams), Z_Construct_UFunction_UMenu_HostButtonClicked_Statics::Function_MetaDataParams) };
UFunction* Z_Construct_UFunction_UMenu_HostButtonClicked()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UMenu_HostButtonClicked_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UMenu::execHostButtonClicked)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HostButtonClicked();
	P_NATIVE_END;
}
// End Class UMenu Function HostButtonClicked

// Begin Class UMenu Function JoinButtonClicked
struct Z_Construct_UFunction_UMenu_JoinButtonClicked_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Menu.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UMenu_JoinButtonClicked_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UMenu, nullptr, "JoinButtonClicked", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_JoinButtonClicked_Statics::Function_MetaDataParams), Z_Construct_UFunction_UMenu_JoinButtonClicked_Statics::Function_MetaDataParams) };
UFunction* Z_Construct_UFunction_UMenu_JoinButtonClicked()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UMenu_JoinButtonClicked_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UMenu::execJoinButtonClicked)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->JoinButtonClicked();
	P_NATIVE_END;
}
// End Class UMenu Function JoinButtonClicked

// Begin Class UMenu Function MenuSetup
struct Z_Construct_UFunction_UMenu_MenuSetup_Statics
{
	struct Menu_eventMenuSetup_Parms
	{
		int32 numberOfPublicConnections;
		FString matchType;
		FString lobbyPath;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "CPP_Default_lobbyPath", "/Game/ThirdPerson/Maps/L_Lobby" },
		{ "CPP_Default_matchType", "FreeForAll" },
		{ "CPP_Default_numberOfPublicConnections", "4" },
		{ "ModuleRelativePath", "Public/Menu.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_numberOfPublicConnections;
	static const UECodeGen_Private::FStrPropertyParams NewProp_matchType;
	static const UECodeGen_Private::FStrPropertyParams NewProp_lobbyPath;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UMenu_MenuSetup_Statics::NewProp_numberOfPublicConnections = { "numberOfPublicConnections", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Menu_eventMenuSetup_Parms, numberOfPublicConnections), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UMenu_MenuSetup_Statics::NewProp_matchType = { "matchType", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Menu_eventMenuSetup_Parms, matchType), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UMenu_MenuSetup_Statics::NewProp_lobbyPath = { "lobbyPath", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Menu_eventMenuSetup_Parms, lobbyPath), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UMenu_MenuSetup_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMenu_MenuSetup_Statics::NewProp_numberOfPublicConnections,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMenu_MenuSetup_Statics::NewProp_matchType,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMenu_MenuSetup_Statics::NewProp_lobbyPath,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_MenuSetup_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UMenu_MenuSetup_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UMenu, nullptr, "MenuSetup", nullptr, nullptr, Z_Construct_UFunction_UMenu_MenuSetup_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_MenuSetup_Statics::PropPointers), sizeof(Z_Construct_UFunction_UMenu_MenuSetup_Statics::Menu_eventMenuSetup_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_MenuSetup_Statics::Function_MetaDataParams), Z_Construct_UFunction_UMenu_MenuSetup_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UMenu_MenuSetup_Statics::Menu_eventMenuSetup_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UMenu_MenuSetup()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UMenu_MenuSetup_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UMenu::execMenuSetup)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_numberOfPublicConnections);
	P_GET_PROPERTY(FStrProperty,Z_Param_matchType);
	P_GET_PROPERTY(FStrProperty,Z_Param_lobbyPath);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->MenuSetup(Z_Param_numberOfPublicConnections,Z_Param_matchType,Z_Param_lobbyPath);
	P_NATIVE_END;
}
// End Class UMenu Function MenuSetup

// Begin Class UMenu Function OnCreateSession
struct Z_Construct_UFunction_UMenu_OnCreateSession_Statics
{
	struct Menu_eventOnCreateSession_Parms
	{
		bool wasSuccessful;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "//\n// Callbacks for the delegates on the MultiplayerSessionSubsystem\n//\n" },
#endif
		{ "ModuleRelativePath", "Public/Menu.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Callbacks for the delegates on the MultiplayerSessionSubsystem" },
#endif
	};
#endif // WITH_METADATA
	static void NewProp_wasSuccessful_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_wasSuccessful;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
void Z_Construct_UFunction_UMenu_OnCreateSession_Statics::NewProp_wasSuccessful_SetBit(void* Obj)
{
	((Menu_eventOnCreateSession_Parms*)Obj)->wasSuccessful = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UMenu_OnCreateSession_Statics::NewProp_wasSuccessful = { "wasSuccessful", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(Menu_eventOnCreateSession_Parms), &Z_Construct_UFunction_UMenu_OnCreateSession_Statics::NewProp_wasSuccessful_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UMenu_OnCreateSession_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMenu_OnCreateSession_Statics::NewProp_wasSuccessful,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnCreateSession_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UMenu_OnCreateSession_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UMenu, nullptr, "OnCreateSession", nullptr, nullptr, Z_Construct_UFunction_UMenu_OnCreateSession_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnCreateSession_Statics::PropPointers), sizeof(Z_Construct_UFunction_UMenu_OnCreateSession_Statics::Menu_eventOnCreateSession_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnCreateSession_Statics::Function_MetaDataParams), Z_Construct_UFunction_UMenu_OnCreateSession_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UMenu_OnCreateSession_Statics::Menu_eventOnCreateSession_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UMenu_OnCreateSession()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UMenu_OnCreateSession_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UMenu::execOnCreateSession)
{
	P_GET_UBOOL(Z_Param_wasSuccessful);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnCreateSession(Z_Param_wasSuccessful);
	P_NATIVE_END;
}
// End Class UMenu Function OnCreateSession

// Begin Class UMenu Function OnDestroySession
struct Z_Construct_UFunction_UMenu_OnDestroySession_Statics
{
	struct Menu_eventOnDestroySession_Parms
	{
		bool wasSuccessful;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Menu.h" },
	};
#endif // WITH_METADATA
	static void NewProp_wasSuccessful_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_wasSuccessful;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
void Z_Construct_UFunction_UMenu_OnDestroySession_Statics::NewProp_wasSuccessful_SetBit(void* Obj)
{
	((Menu_eventOnDestroySession_Parms*)Obj)->wasSuccessful = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UMenu_OnDestroySession_Statics::NewProp_wasSuccessful = { "wasSuccessful", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(Menu_eventOnDestroySession_Parms), &Z_Construct_UFunction_UMenu_OnDestroySession_Statics::NewProp_wasSuccessful_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UMenu_OnDestroySession_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMenu_OnDestroySession_Statics::NewProp_wasSuccessful,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnDestroySession_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UMenu_OnDestroySession_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UMenu, nullptr, "OnDestroySession", nullptr, nullptr, Z_Construct_UFunction_UMenu_OnDestroySession_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnDestroySession_Statics::PropPointers), sizeof(Z_Construct_UFunction_UMenu_OnDestroySession_Statics::Menu_eventOnDestroySession_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnDestroySession_Statics::Function_MetaDataParams), Z_Construct_UFunction_UMenu_OnDestroySession_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UMenu_OnDestroySession_Statics::Menu_eventOnDestroySession_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UMenu_OnDestroySession()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UMenu_OnDestroySession_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UMenu::execOnDestroySession)
{
	P_GET_UBOOL(Z_Param_wasSuccessful);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnDestroySession(Z_Param_wasSuccessful);
	P_NATIVE_END;
}
// End Class UMenu Function OnDestroySession

// Begin Class UMenu Function OnStartSession
struct Z_Construct_UFunction_UMenu_OnStartSession_Statics
{
	struct Menu_eventOnStartSession_Parms
	{
		bool wasSuccessful;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Menu.h" },
	};
#endif // WITH_METADATA
	static void NewProp_wasSuccessful_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_wasSuccessful;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
void Z_Construct_UFunction_UMenu_OnStartSession_Statics::NewProp_wasSuccessful_SetBit(void* Obj)
{
	((Menu_eventOnStartSession_Parms*)Obj)->wasSuccessful = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UMenu_OnStartSession_Statics::NewProp_wasSuccessful = { "wasSuccessful", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(Menu_eventOnStartSession_Parms), &Z_Construct_UFunction_UMenu_OnStartSession_Statics::NewProp_wasSuccessful_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UMenu_OnStartSession_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMenu_OnStartSession_Statics::NewProp_wasSuccessful,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnStartSession_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UMenu_OnStartSession_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UMenu, nullptr, "OnStartSession", nullptr, nullptr, Z_Construct_UFunction_UMenu_OnStartSession_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnStartSession_Statics::PropPointers), sizeof(Z_Construct_UFunction_UMenu_OnStartSession_Statics::Menu_eventOnStartSession_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UMenu_OnStartSession_Statics::Function_MetaDataParams), Z_Construct_UFunction_UMenu_OnStartSession_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UMenu_OnStartSession_Statics::Menu_eventOnStartSession_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UMenu_OnStartSession()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UMenu_OnStartSession_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UMenu::execOnStartSession)
{
	P_GET_UBOOL(Z_Param_wasSuccessful);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnStartSession(Z_Param_wasSuccessful);
	P_NATIVE_END;
}
// End Class UMenu Function OnStartSession

// Begin Class UMenu
void UMenu::StaticRegisterNativesUMenu()
{
	UClass* Class = UMenu::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "HostButtonClicked", &UMenu::execHostButtonClicked },
		{ "JoinButtonClicked", &UMenu::execJoinButtonClicked },
		{ "MenuSetup", &UMenu::execMenuSetup },
		{ "OnCreateSession", &UMenu::execOnCreateSession },
		{ "OnDestroySession", &UMenu::execOnDestroySession },
		{ "OnStartSession", &UMenu::execOnStartSession },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UMenu);
UClass* Z_Construct_UClass_UMenu_NoRegister()
{
	return UMenu::StaticClass();
}
struct Z_Construct_UClass_UMenu_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "Menu.h" },
		{ "ModuleRelativePath", "Public/Menu.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HostButton_MetaData[] = {
		{ "BindWidget", "" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Menu.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_JoinButton_MetaData[] = {
		{ "BindWidget", "" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Menu.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_HostButton;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_JoinButton;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UMenu_HostButtonClicked, "HostButtonClicked" }, // 128811646
		{ &Z_Construct_UFunction_UMenu_JoinButtonClicked, "JoinButtonClicked" }, // 3398739101
		{ &Z_Construct_UFunction_UMenu_MenuSetup, "MenuSetup" }, // 1051241026
		{ &Z_Construct_UFunction_UMenu_OnCreateSession, "OnCreateSession" }, // 931158232
		{ &Z_Construct_UFunction_UMenu_OnDestroySession, "OnDestroySession" }, // 2308476973
		{ &Z_Construct_UFunction_UMenu_OnStartSession, "OnStartSession" }, // 2580713663
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMenu>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UMenu_Statics::NewProp_HostButton = { "HostButton", nullptr, (EPropertyFlags)0x0040000000080008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMenu, HostButton), Z_Construct_UClass_UButton_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HostButton_MetaData), NewProp_HostButton_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UMenu_Statics::NewProp_JoinButton = { "JoinButton", nullptr, (EPropertyFlags)0x0040000000080008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMenu, JoinButton), Z_Construct_UClass_UButton_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_JoinButton_MetaData), NewProp_JoinButton_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UMenu_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMenu_Statics::NewProp_HostButton,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMenu_Statics::NewProp_JoinButton,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMenu_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UMenu_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UUserWidget,
	(UObject* (*)())Z_Construct_UPackage__Script_MultiplayerSessions,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMenu_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMenu_Statics::ClassParams = {
	&UMenu::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UMenu_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UMenu_Statics::PropPointers),
	0,
	0x00B010A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMenu_Statics::Class_MetaDataParams), Z_Construct_UClass_UMenu_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UMenu()
{
	if (!Z_Registration_Info_UClass_UMenu.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMenu.OuterSingleton, Z_Construct_UClass_UMenu_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMenu.OuterSingleton;
}
template<> MULTIPLAYERSESSIONS_API UClass* StaticClass<UMenu>()
{
	return UMenu::StaticClass();
}
UMenu::UMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UMenu);
UMenu::~UMenu() {}
// End Class UMenu

// Begin Registration
struct Z_CompiledInDeferFile_FID_UnrealEngine_Projects_MultiplayerShooter___Copy_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMenu, UMenu::StaticClass, TEXT("UMenu"), &Z_Registration_Info_UClass_UMenu, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMenu), 3678997218U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_UnrealEngine_Projects_MultiplayerShooter___Copy_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_3305482818(TEXT("/Script/MultiplayerSessions"),
	Z_CompiledInDeferFile_FID_UnrealEngine_Projects_MultiplayerShooter___Copy_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_UnrealEngine_Projects_MultiplayerShooter___Copy_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_Menu_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
