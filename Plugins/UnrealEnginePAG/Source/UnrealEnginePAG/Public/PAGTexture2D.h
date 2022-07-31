// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PAGTexture2D.generated.h"

namespace pag
{
	class PAGFile;
	class PAGPlayer;
	class PAGSurface;
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPAGTextureDelegate);

/**
 * 
 */
UCLASS(BlueprintType, Category = PAGTexture2D, hideCategories = (Adjustments, Compression, LevelOfDetail, Compositing))
class UNREALENGINEPAG_API UPAGTexture2D : public UTexture
{
	GENERATED_BODY()
	
public:
	
	friend class FPAGTextureResource;

	UPAGTexture2D(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostLoad() override;

	virtual void BeginDestroy() override;
	
	//~ Begin UTexture Interface.
	virtual float GetSurfaceWidth() const override;
	virtual float GetSurfaceHeight() const override;
	virtual float GetSurfaceDepth() const override { return 0; }
	virtual uint32 GetSurfaceArraySize() const override { return 0; }
	virtual FTextureResource* CreateResource() override;
	virtual EMaterialValueType GetMaterialType() const override { return MCT_Texture2D; }
	virtual uint32 CalcTextureMemorySizeEnum(ETextureMipCount Enum) const override;
	//~ End UTexture Interface.

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	virtual void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) override;
	//~ End UObject Interface.
	
	bool ImportPAG(const uint8* Buffer, uint32 BufferSize);
	bool LoadPAG();
	void ReadFrameData();
	void Reset();
	
private:
	void Finish();
	void Release();
	
public:
	/** Delegate */
	UPROPERTY(BlueprintAssignable)
	FPAGTextureDelegate OnFinished;

private:
	int32 TotalFrames;
	
	uint32 ContentHeight;
	uint32 ContentWidth;
	
	std::shared_ptr<pag::PAGFile> PAGFile;
	std::shared_ptr<pag::PAGPlayer> PAGPlayer;
	std::shared_ptr<pag::PAGSurface> PAGSurface;

	UPROPERTY()
	TArray<uint8> RawData;
	
	/** PAG property */
public:
	/* 播放/暂停 */
	UPROPERTY(EditAnywhere, Category = "Editor")
	bool bPlay = true;
	
	/** 设置循环 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PAG")
	bool bLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PAG")
	bool bAutoRelease = true;
protected:
	/** 帧率 */
	UPROPERTY(VisibleAnywhere, Category = "PAG")
	float FrameRate = 0.f;

/** PAG functions */
public:
	
	/**
	 * @brief 设置动画当前帧位置 
	 * @param Frame 
	 */
	UFUNCTION(BlueprintCallable, Category = "PAGTexture2D")
	void SetFrame(int32 Frame);


	/**
	 * @brief 获取当前帧位置 
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "PAGTexture2D")
	int32 GetFrame() const;

	/**
	 * @brief 获取帧数量
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "PAGTexture2D")
	int32 GetTotalFrame() const;
	
	/**
	 * @brief 设置进度
	 * @param Percent 
	 */
	UFUNCTION(BlueprintCallable, Category = "PAGTexture2D")
	void SetProgress(float Percent);

	
	/**
	 * @brief 停止播放
	 */
	UFUNCTION(BlueprintCallable, Category = "PAGTexture2D")
	void Stop();

	/**
	 * @brief 播放
	 */
	UFUNCTION(BlueprintCallable, Category = "PAGTexture2D")
	void Play();

	UFUNCTION(BlueprintPure, Category = "PAGTexture2D")
	FORCEINLINE bool IsPlaying() const { return bIsPlaying; }

	/**
	 * @brief 从头开始播放
	 */
	UFUNCTION(BlueprintCallable, Category = "PAGTexture2D")
	void PlayFromStart();

	/**
	 * @brief 获取时长
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "PAGTexture2D")
	float GetDuration() const;

	/**
	 * @brief 设置文本
	 * @param InText
	 * @param TextIndex
	 * return true if success
	 */
	UFUNCTION(BlueprintCallable, Category = "PAGTexture2D")
	bool SetText(const FString& InText, int32 TextIndex = 0);
	
private:
	bool bIsPlaying;
	uint8* FrameData = nullptr;
	int32 CurrentFrame = 0;
	bool bFinished;

	mutable FCriticalSection PlayingCS;
};

