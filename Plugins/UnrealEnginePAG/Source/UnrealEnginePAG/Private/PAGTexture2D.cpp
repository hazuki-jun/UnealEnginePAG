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
	LODGroup = TEXTUREGROUP_UI;
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
	return Super::CalcTextureMemorySizeEnum(Enum);
}

#if WITH_EDITOR
void UPAGTexture2D::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (bIsPlaying != bPlayInEditor)
	{
		bIsPlaying = bPlayInEditor;
	}

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
}

void UPAGTexture2D::PostLoad()
{
	ParsePAG();
	bPlayInEditor = false;
	bFinished = false;
	SetFrame(0);
	Super::PostLoad();
}

void UPAGTexture2D::BeginDestroy()
{
	Release();
	Super::BeginDestroy();
}

bool UPAGTexture2D::ImportPAG(const uint8* Buffer, uint32 BufferSize)
{
	RawData.SetNumUninitialized(BufferSize);
	FMemory::Memcpy(RawData.GetData(), Buffer, BufferSize);
	return ParsePAG();
}

bool UPAGTexture2D::ParsePAG()
{
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
		FrameData = new uint8[ContentHeight * ContentWidth * 4];
		
		if (PAGSurface == nullptr)
		{
			PAGSurface = pag::PAGSurface::MakeOffscreen(ContentWidth, ContentHeight);
			if (PAGSurface == nullptr)
			{
				return false;
			}
		}
	
		if (PAGPlayer == nullptr)
		{
			PAGPlayer = std::make_shared<pag::PAGPlayer>();
			PAGPlayer->setSurface(PAGSurface);
			PAGPlayer->setComposition(PAGFile);
	
			SetProgress(0);
		}
	}
	
	return true;
}

void UPAGTexture2D::Release()
{
	OnFinished.Clear();
	Finish();
	Reset();
	if (FrameData)
	{
		delete[] FrameData;
	}
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
		if (bIsLoop)
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
	bIsPlaying = false;
}

void UPAGTexture2D::Play()
{
	bIsPlaying = true;
}

void UPAGTexture2D::PlayFromStart()
{
	bIsPlaying = false;
	bFinished = false;
	SetFrame(0);
	bIsPlaying = true;
}

float UPAGTexture2D::GetDuration() const
{
	return PAGPlayer ? PAGPlayer->duration() : 0.f;
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
	
	FMemory::Memzero(FrameData, ContentWidth * ContentHeight * 4);
	if (PAGSurface)
	{
		PAGSurface->readPixels(pag::ColorType::BGRA_8888, pag::AlphaType::Premultiplied, FrameData, PAGFile->width() * 4);
	}
}

void UPAGTexture2D::Reset()
{
	Stop();
	PAGFile.reset();
	PAGFile = nullptr;
	
	PAGPlayer.reset();
	PAGPlayer = nullptr;
	
	PAGSurface.reset();
	PAGSurface = nullptr;
}

void UPAGTexture2D::Finish()
{
	if (bFinished)
	{
		return;
	}
	
	bFinished = true;
	bIsPlaying = false;

	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnFinished.Broadcast();
		OnFinished.Clear();
	});
}

