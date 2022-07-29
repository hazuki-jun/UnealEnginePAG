// Fill out your copyright notice in the Description page of Project Settings.


#include "PAGTextureStatic.h"

#include "PAGTexture2D.h"
#include "Components/Image.h"

UPAGTexture2D* UPAGTextureStatic::GetBrushResource(const UImage* InImage)
{
	if (InImage)
	{
		return Cast<UPAGTexture2D>(InImage->Brush.GetResourceObject());
	}

	return nullptr;
}
