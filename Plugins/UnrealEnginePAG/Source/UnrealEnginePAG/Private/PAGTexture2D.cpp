// Fill out your copyright notice in the Description page of Project Settings.


#include "PAGTexture2D.h"

#include "PAGTextureResource.h"
#include "pag/pag.h"

namespace PAGTexture2D
{
	int64_t TimeToFrame(int64_t Time, float FrameRate)
	{
		return static_cast<int64_t>(floor(Time * FrameRate / 1000000ll));
	}
	
	TArray<uint8> GetBMPHeader(int imageWidth, int imageHeight)
	{
		TArray<uint8> header = {0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0,
									0,    0,    0, 0, 0, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0,  0, 0, 0,
									0,    0,    0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0};
		
		int64_t file_size = static_cast<int64_t>(imageWidth) * static_cast<int64_t>(imageHeight) * 4 + 54;
		header[2] = static_cast<unsigned char>(file_size & 0x000000ff);
		header[3] = (file_size >> 8) & 0x000000ff;
		header[4] = (file_size >> 16) & 0x000000ff;
		header[5] = (file_size >> 24) & 0x000000ff;

		int64_t width = imageWidth;
		header[18] = width & 0x000000ff;
		header[19] = (width >> 8) & 0x000000ff;
		header[20] = (width >> 16) & 0x000000ff;
		header[21] = (width >> 24) & 0x000000ff;

		int64_t height = -imageHeight;
		header[22] = height & 0x000000ff;
		header[23] = (height >> 8) & 0x000000ff;
		header[24] = (height >> 16) & 0x000000ff;
		header[25] = (height >> 24) & 0x000000ff;

		return header;
	}
	
}

using namespace PAGTexture2D;

UPAGTexture2D::UPAGTexture2D(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PAGFile = nullptr;
	PAGPlayer = nullptr;
	PAGSurface = nullptr;
	bIsPlaying = true;
	bFinished = false;
	bLoop = true;
	LODGroup = TEXTUREGROUP_UI;
	
	ContentWidth = 0;
	ContentHeight = 0;
}

float UPAGTexture2D::GetSurfaceWidth() const
{
	return ContentWidth;
}

float UPAGTexture2D::GetSurfaceHeight() const
{
	return ContentHeight;
}

FTextureResource* UPAGTexture2D::CreateResource()
{
	return new FPAGTextureResource(this);
}

uint32 UPAGTexture2D::CalcTextureMemorySizeEnum(ETextureMipCount Enum) const
{
	if(ContentHeight > 0 && ContentWidth > 0) 
	{
		auto Flags = (SRGB ? TexCreate_SRGB : TexCreate_None)  | (bNotOfflineProcessed ? TexCreate_None : TexCreate_OfflineProcessed) | TexCreate_ShaderResource | (bNoTiling ? TexCreate_NoTiling : TexCreate_None);
		uint32 TextureAlign;
		uint64 Size = RHICalcTexture2DPlatformSize(ContentWidth, ContentHeight, PF_B8G8R8A8, 1, 1, Flags, FRHIResourceCreateInfo(TEXT("PAGTexture2D")), TextureAlign);
		return static_cast<uint32>(Size);
	}

	return 4;
}

#if WITH_EDITOR
void UPAGTexture2D::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FScopeLock LockPlaying(&PlayingCS);
	
	bIsPlaying = bPlay;
	
	if (bIsPlaying && bFinished)
	{
		bFinished = false;
		SetFrame(0);
	}
}
#endif

void UPAGTexture2D::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize)
{
	Super::GetResourceSizeEx(CumulativeResourceSize);

	if (!IsCurrentlyVirtualTextured())
	{
		CumulativeResourceSize.AddDedicatedSystemMemoryBytes(RawData.GetAllocatedSize());
	}
}

void UPAGTexture2D::PostLoad()
{
	if (LoadPAG())
	{
		SetFrame(0);
	}

#if WITH_EDITOR
	bPlay = bIsPlaying;
#endif
	
	Super::PostLoad();
}

void UPAGTexture2D::BeginDestroy()
{
	Stop();
	Release();
	Super::BeginDestroy();
}

bool UPAGTexture2D::ImportPAG(const uint8* Buffer, uint32 BufferSize)
{
	RawData.SetNumUninitialized(BufferSize);
	FMemory::Memcpy(RawData.GetData(), Buffer, BufferSize);
	return LoadPAG();
}

bool UPAGTexture2D::LoadPAG()
{
	if (GIsCookerLoadingPackage)
	{
		return false;
	}
	
	if (PAGFile == nullptr)
	{
		PAGFile = pag::PAGFile::Load(RawData.GetData(), RawData.Num());
		if (PAGFile == nullptr)
		{
			return false;
		}
		
		ContentHeight = PAGFile->height();
		ContentWidth = PAGFile->width();
		TotalFrames = (int32)(TimeToFrame(PAGFile->duration(), PAGFile->frameRate()));
		FrameRate = PAGFile->frameRate();

		if (PAGSurface == nullptr)
		{
			PAGSurface = pag::PAGSurface::MakeOffscreen(ContentWidth, ContentHeight);
			
			if (PAGSurface == nullptr)
			{
				return false;
			}	

			if (PAGPlayer == nullptr)
			{
				PAGPlayer = std::make_shared<pag::PAGPlayer>();
				PAGPlayer->setComposition(PAGFile);
				PAGPlayer->setSurface(PAGSurface);
				SetProgress(0);
			}
		}
	}
	
	return true;
}


void UPAGTexture2D::Release()
{
	if (OnFinished.IsBound())
	{
		OnFinished.Clear();
	}
	
	if (FrameData)
	{
		delete[] FrameData;
	}
	
	Reset();
}

void UPAGTexture2D::SetFrame(int32 Frame)
{
	CurrentFrame = Frame;
	SetProgress(CurrentFrame * 1.f / TotalFrames);
}

int32 UPAGTexture2D::GetFrame() const
{
	// return PAGPlayer->getProgress() * GetTotalFrame();
	return CurrentFrame;
}

int32 UPAGTexture2D::GetTotalFrame() const
{
	return TotalFrames;
}

void UPAGTexture2D::SetProgress(float Percent)
{
	if (Percent > 1.f)
	{
		if (bLoop)
		{
			SetFrame(0);
		}
		else
		{
			Finish();
			return;
		}
	}
	
	if (PAGPlayer)
	{
		PAGPlayer->setProgress(Percent);
		auto Status = PAGPlayer->flush();	
	}
}

void UPAGTexture2D::Stop()
{
	FScopeLock LockPlaying(&PlayingCS);
	bIsPlaying = false;
}

void UPAGTexture2D::Play()
{
	FScopeLock LockPlaying(&PlayingCS);
	bIsPlaying = true;
}

void UPAGTexture2D::PlayFromStart()
{
	FScopeLock LockPlaying(&PlayingCS);
	bIsPlaying = false;
	bFinished = false;
	SetFrame(0);
	bIsPlaying = true;
}

float UPAGTexture2D::GetDuration() const
{
	return PAGPlayer ? PAGPlayer->duration() : 0.f;
}

bool UPAGTexture2D::SetText(const FString& InText, int32 TextIndex)
{
	if (PAGFile)
	{
		const auto TextNum = PAGFile->numTexts(); 
		if (TextIndex >= 0 && TextIndex < TextNum)
		{
			if (auto TextDoc = PAGFile->getTextData(TextIndex))
			{
				TextDoc->text = TCHAR_TO_UTF8(*InText);
				PAGFile->replaceText(TextIndex, TextDoc);
				return true;
			}
		}
	}

	return false;
}

void UPAGTexture2D::ReadFrameData()
{
	if (ContentHeight <= 0 || ContentWidth <= 0)
	{
		return;
	}
	
	if (FrameData == nullptr)
	{
		FrameData =  new uint8[ContentHeight * ContentWidth * 4];
	}
	
	if (PAGSurface)
	{
		FMemory::Memzero(FrameData, ContentWidth * ContentHeight * 4);
		PAGSurface->readPixels(pag::ColorType::BGRA_8888, pag::AlphaType::Premultiplied, FrameData, PAGFile->width() * 4);
	}
}

void UPAGTexture2D::Reset()
{
	if (PAGFile)
	{
		PAGFile.reset();
		PAGFile = nullptr;
	}

	if (PAGPlayer)
	{
		PAGPlayer.reset();
		PAGPlayer = nullptr;
	}

	if (PAGSurface)
	{
		PAGSurface.reset();
		PAGSurface = nullptr;
	}
}

void UPAGTexture2D::Finish()
{
	if (bFinished)
	{
		return;
	}
	
	bFinished = true;
	bIsPlaying = false;

#if WITH_EDITOR
	SetFrame(0);
#endif

#if WITH_EDITORONLY_DATA
	bPlay = false;
#endif
	
	if (OnFinished.IsBound())
	{
		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnFinished.Broadcast();
			OnFinished.Clear();
		});
	}
#if !WITH_EDITOR
	if (bAutoRelease)
	{
		Release();
	}
#endif
}

