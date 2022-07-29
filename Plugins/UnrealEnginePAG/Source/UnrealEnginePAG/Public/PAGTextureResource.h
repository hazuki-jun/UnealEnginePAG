#pragma once

#include "CoreMinimal.h"
#include "TextureResource.h"

class FPAGTextureResource : public FTextureResource, public FTickableObjectRenderThread
{
public:
	FPAGTextureResource(class UPAGTexture2D* InOwner);
	~FPAGTextureResource();
	
	//~ Begin FTextureResource Interface.
	 virtual uint32 GetSizeX() const override;
	 virtual uint32 GetSizeY() const override;
	 virtual void InitRHI() override;
	 virtual void ReleaseRHI() override;
	//~ End FTextureResource Interface.


	//~ Begin FTickableObjectRenderThread Interface.
	 virtual void Tick(float DeltaTime) override;
	 virtual bool IsTickable() const override
	 {
	 	return true;
	 }
	 virtual TStatId GetStatId() const
	 {
	 	RETURN_QUICK_DECLARE_CYCLE_STAT(UAnimatedTexture2D, STATGROUP_Tickables);
	 }
	//~ End FTickableObjectRenderThread Interface.
	
private:
	void CreateSamplerStates(float MipMapBias);
	
	void TickTexture(float DeltaTime);

	void CreateTexture();
	
	/**
	 * Writes the data for a single mip-level into a destination buffer.
	 * @param MipIndex	The index of the mip-level to read.
	 * @param Dest		The address of the destination buffer to receive the mip-level's data.
	 * @param DestPitch	Number of bytes per row
	 */
	void GetData(void* Dest,uint32 DestPitch );
	
private:
	TObjectPtr<class UPAGTexture2D> Owner;
	float Time = 0.f;
};
