// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Player/SKTPlayerHUD.h"
#include "Components/TextBlock.h"

void USKTPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    RadicalMessages = {
    TEXT("AWESOME!"),
    TEXT("INCREDIBLE!"),
    TEXT("UNSTOPPABLE!"),
    TEXT("LEGENDARY!"),
    TEXT("WICKED!"),
    TEXT("AMAZING!"),
    TEXT("EPIC WIN!"),
    TEXT("WOWZA!"),
    TEXT("YOU ROCK!"),
    TEXT("BEAST MODE!"),
    TEXT("PHENOMENAL!"),
    TEXT("RADICAL!"),
    TEXT("ON FIRE!")
    };

    if (RadicalMessageText)
    {
        RadicalMessageText->SetText(FText::FromString(TEXT("")));
    }

    if (PointsText)
    {
        PointsText->SetColorAndOpacity(FSlateColor(PointsColor));
        PointsText->SetText(FText::FromString(TEXT("SCORE\n0")));
    }
}

void USKTPlayerHUD::TriggerRandomRadicalMessage()
{
    if (RadicalMessages.Num() == 0 || !RadicalMessageText) return;

    int32 Index = FMath::RandRange(0, RadicalMessages.Num() - 1);
    FullRadicalMessage = RadicalMessages[Index];
    CurrentRadicalMessage = "";
    RadicalCharIndex = 0;

    GetWorld()->GetTimerManager().ClearTimer(RadicalTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(RadicalDisappearTimerHandle);

    if (RadicalColors.Num() > 0)
    {
        int32 ColorIndex = FMath::RandRange(0, RadicalColors.Num() - 1);
        CurrentRadicalColor = RadicalColors[ColorIndex];
    }
    else
    {
        CurrentRadicalColor = FLinearColor::Red;
    }

    if (RadicalMessageText)
    {
        RadicalMessageText->SetColorAndOpacity(FSlateColor(CurrentRadicalColor));
    }

    GetWorld()->GetTimerManager().SetTimer(RadicalTimerHandle, [this]()
        {
            ShowNextRadicalChar();
        }, 0.05f, true);
}

void USKTPlayerHUD::ShowNextRadicalChar()
{
    if (RadicalCharIndex >= FullRadicalMessage.Len())
    {
        GetWorld()->GetTimerManager().ClearTimer(RadicalTimerHandle);

        GetWorld()->GetTimerManager().SetTimer(RadicalDisappearTimerHandle, [this]()
            {
                if (RadicalMessageText)
                {
                    RadicalMessageText->SetText(FText::FromString(""));
                }
            }, RadicalMessageDuration, false);

        return;
    }

    CurrentRadicalMessage.AppendChar(FullRadicalMessage[RadicalCharIndex]);
    RadicalCharIndex++;

    if (RadicalMessageText)
    {
        RadicalMessageText->SetText(FText::FromString(CurrentRadicalMessage));
        RadicalMessageText->SetColorAndOpacity(FSlateColor(CurrentRadicalColor));
    }
}

void USKTPlayerHUD::IncrementScore()
{
    if (CurrentDisplayPoints < TargetPoints)
    {
        CurrentDisplayPoints += IncrementStep;
        if (CurrentDisplayPoints > TargetPoints)
        {
            CurrentDisplayPoints = TargetPoints;
        }

        if (CurrentDisplayPoints >= 1000)
        {
            TriggerRandomRadicalMessage();
        }

        PointsText->SetText(FText::FromString(FString::Printf(TEXT("SCORE\n%d"), CurrentDisplayPoints)));
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(IncrementTimerHandle);
    }    
}

void USKTPlayerHUD::UpdatePoints(int32 NewPoints)
{
    if (!PointsText) return;

    TargetPoints = NewPoints;

    if (GetWorld()->GetTimerManager().IsTimerActive(IncrementTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(IncrementTimerHandle);
    }

    GetWorld()->GetTimerManager().SetTimer(IncrementTimerHandle, this, &USKTPlayerHUD::IncrementScore, IncrementDelay, true);    
}