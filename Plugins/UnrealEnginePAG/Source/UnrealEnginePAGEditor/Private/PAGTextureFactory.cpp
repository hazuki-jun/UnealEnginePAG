// Fill out your copyright notice in the Description page of Project Settings.


#include "PAGTextureFactory.h"

#include "Editor.h"
#include "LogChannel.h"
#include "PAGTexture2D.h"

UPAGTextureFactory::UPAGTextureFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UPAGTexture2D::StaticClass();

	Formats.Add(TEXT("pag; pag file"));

	bCreateNew = false;
	bEditorImport = true;

	// Required to checkbefore UReimportTextureFactory
	ImportPriority = DefaultImportPriority + 1;
}

bool UPAGTextureFactory::DoesSupportClass(UClass* Class)
{
	return Class == UPAGTexture2D::StaticClass();
}

bool UPAGTextureFactory::FactoryCanImport(const FString& Filename)
{
	FString Extension = FPaths::GetExtension(Filename, true);

	return Extension.Compare(TEXT(".pag"), ESearchCase::IgnoreCase) == 0;
}

UObject* UPAGTextureFactory::FactoryCreateBinary(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn)
{
	check(Type);
	check(Class == UPAGTexture2D::StaticClass());

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, Class, InParent, Name, Type);

	// if the texture already exists, remember the user settings
	UPAGTexture2D* ExistingTexture = FindObject<UPAGTexture2D>(InParent, *Name.ToString());
	if (ExistingTexture) {
		// TODO: use existing object settings
	}

	//FTextureReferenceReplacer RefReplacer(ExistingTexture);

	// call parent method to create/overwrite anim texture object
	UPAGTexture2D* PAGTexture = Cast<UPAGTexture2D>(
		CreateOrOverwriteAsset(Class, InParent, Name, Flags)
		);
	if (PAGTexture == nullptr) {
		UE_LOG(LogPAGTexture, Error, TEXT("Create PAG Texture FAILED, Name=%s."), *(Name.ToString()));
		return nullptr;
	}

	if (!PAGTexture->ImportPAG(Buffer, BufferEnd - Buffer))
	{
		UE_LOG(LogPAGTexture, Error, TEXT("Import PAG FAILED, Name=%s."), *(Name.ToString()));
		PAGTexture->Reset();
	}
	else
	{
		PAGTexture->UpdateResource();
	}
	
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, Class, InParent, Name, Type);
	
	// Invalidate any materials using the newly imported texture. (occurs if you import over an existing texture)
	PAGTexture->PostEditChange();

	return PAGTexture;
}
