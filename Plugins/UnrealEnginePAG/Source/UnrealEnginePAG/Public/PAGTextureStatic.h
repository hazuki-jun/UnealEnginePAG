// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PAGTextureStatic.generated.h"

/**
 * 
 */
UCLASS()
class UNREALENGINEPAG_API UPAGTextureStatic : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief 获取 PAGTexture2D
	 * @param InImage 
	 * @return PAGTexture2D
	 */
	UFUNCTION(BlueprintPure, Category = "UPAGTextureStatic")
	static class UPAGTexture2D* GetBrushResource(const class UImage* InImage);
};

