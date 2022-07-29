#include "PAGTextureResource.h"

#include "PAGTexture2D.h"
#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "Rendering/Texture2DResource.h"

FPAGTextureResource::FPAGTextureResource(UPAGTexture2D* InOwner)
: FTickableObjectRenderThread(false, true)
, Owner(InOwner)
{
	TextureReferenceRHI = InOwner->TextureReference.TextureReferenceRHI;
}

FPAGTextureResource::~FPAGTextureResource()
{
	
}

uint32 FPAGTextureResource::GetSizeX() const
{
	return Owner ? Owner->ContentWidth : 0;
}

uint32 FPAGTextureResource::GetSizeY() const
{
	return Owner ? Owner->ContentHeight : 0;
}

void FPAGTextureResource::InitRHI()
{
	auto CreationFlags = (Owner->SRGB ? TexCreate_SRGB : TexCreate_None)  | (Owner->bNotOfflineProcessed ? TexCreate_None : TexCreate_OfflineProcessed) | TexCreate_ShaderResource | (Owner->bNoTiling ? TexCreate_NoTiling : TexCreate_None);
	
	FRHIResourceCreateInfo CreateInfo(TEXT("PAGTexture2D"));
	TextureRHI = RHICreateTexture2D(GetSizeX(), GetSizeY(), PF_B8G8R8A8, 1, 1, CreationFlags, CreateInfo);
	TextureRHI->SetName(Owner->GetFName());
	RHIUpdateTextureReference(TextureReferenceRHI, TextureRHI);
	CreateSamplerStates(1);
	CreateTexture();
	Register();
}

void FPAGTextureResource::CreateSamplerStates(float MipMapBias)
{
	const auto Filter = static_cast<ESamplerFilter>(UDeviceProfileManager::Get().GetActiveProfile()->GetTextureLODSettings()->GetSamplerFilter(Owner));
	
	FSamplerStateInitializerRHI SamplerStateInitializer(Filter, AM_Wrap, AM_Wrap, AM_Wrap, MipMapBias);
	SamplerStateRHI = GetOrCreateSamplerState(SamplerStateInitializer);

	FSamplerStateInitializerRHI DeferredPassSamplerStateInitializer(Filter, AM_Wrap, AM_Wrap, AM_Wrap, MipMapBias, 1, 0, 2);
	DeferredPassSamplerStateRHI = GetOrCreateSamplerState(DeferredPassSamplerStateInitializer);
}

void FPAGTextureResource::ReleaseRHI()
{
	Unregister();
	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, nullptr);
	FTextureResource::ReleaseRHI();
}

void FPAGTextureResource::Tick(float DeltaTime)
{
	if (!IsValid(Owner))
	{
		return;
	}
	
	const float Duration = FApp::GetCurrentTime() - Owner->GetLastRenderTimeForStreaming();
	if (Owner->IsPlaying() && Duration < 1.5f)
	{
		TickTexture(DeltaTime);
	}
}

void FPAGTextureResource::TickTexture(float DeltaTime)
{
	auto CurrentFrame = Owner->GetFrame();
	if (CurrentFrame == 0)
	{
		Time = 0.f;
	}

	if (Time >= CurrentFrame / Owner->FrameRate)
	{
		Owner->SetFrame(Owner->GetFrame() + 1);
		CreateTexture();
	}
	
	Time += DeltaTime;
}

void FPAGTextureResource::CreateTexture()
{
	FTexture2DRHIRef Texture2DRHI = TextureRHI->GetTexture2D();
	uint32 DestPitch;
	void* TheMipData = RHILockTexture2D(Texture2DRHI, 0, RLM_WriteOnly, DestPitch, false );
	GetData(TheMipData, DestPitch);
	RHIUnlockTexture2D( Texture2DRHI, 0, false );
}

void FPAGTextureResource::GetData(void* Dest, uint32 DestPitch)
{
	constexpr auto PixelFormat = PF_B8G8R8A8; 
	Owner->ReadFrameData();

	// for platforms that returned 0 pitch from Lock, we need to just use the bulk data directly, never do 
	// runtime block size checking, conversion, or the like
	if (DestPitch == 0)
	{
		int32 PixelSize = GetSizeY() * GetSizeX() * GPixelFormats[PixelFormat].BlockBytes;
		FMemory::Memcpy(Dest, Owner->FrameData, PixelSize);
	}
	else
	{
		const uint32 BlockSizeX = GPixelFormats[PixelFormat].BlockSizeX;		// Block width in pixels
		const uint32 BlockBytes = GPixelFormats[PixelFormat].BlockBytes;
		uint32 NumColumns		= (GetSizeX() + BlockSizeX - 1) / BlockSizeX;	// Num-of columns in the source data (in blocks)
		const uint32 SrcPitch   = NumColumns * BlockBytes;						// Num-of bytes per row in the source data

		// Copy the texture data.
		CopyTextureData2D(Owner->FrameData, Dest, GetSizeY(), PixelFormat,SrcPitch,DestPitch);
	}

	// FrameData.Empty();
}

