// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PAGTextureFactory.generated.h"

/**
 * 
 */
UCLASS()
class UNREALENGINEPAGEDITOR_API UPAGTextureFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	
	//~ Begin UFactory Interface
	virtual bool DoesSupportClass(UClass* Class) override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateBinary(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;
	//~ End UFactory Interface
};
