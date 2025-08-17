// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include <Components/TextBlock.h>
#include "SKTPlayerHUD.generated.h"

USTRUCT()
struct FSpeedLevel
{
    GENERATED_BODY()

    float MaxSpeed;
    FString Name;
    FLinearColor Color;
};

/**
 * 
 */
UCLASS()
class PROSKATER_API USKTPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Points")
    void UpdatePoints(int32 NewPoints);

    void UpdateSpeed(float CurrentSpeed);

    UPROPERTY(EditAnywhere, Category = "UI|Speed")
    TArray<FSpeedLevel> SpeedLevels;

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PointsText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RadicalMessageText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SpeedText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points|Style")
    FLinearColor PointsColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points|Radical")
    TArray<FString> RadicalMessages;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points|Radical")
    TArray<FLinearColor> RadicalColors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points|Radical")
    float RadicalMessageDuration = 2.5f;

    virtual void NativeConstruct() override;

private:
    int32 TargetPoints = 0;
    int32 CurrentDisplayPoints = 0;

    float IncrementDelay = 0.02f;
    int32 IncrementStep = 1;

    FTimerHandle IncrementTimerHandle;
    FTimerHandle RadicalDisappearTimerHandle;

    FString FullRadicalMessage;
    FString CurrentRadicalMessage;
    int32 RadicalCharIndex;
    FTimerHandle RadicalTimerHandle;

    FLinearColor CurrentRadicalColor;

    void TriggerRandomRadicalMessage();

    void ShowNextRadicalChar();

    void IncrementScore();
};
