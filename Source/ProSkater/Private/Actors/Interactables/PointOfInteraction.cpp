// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactables/PointOfInteraction.h"
#include "Components/SphereComponent.h"
#include "Character/SKTBasePlayer.h"
#include "Engine/Engine.h"

// Sets default values
APointOfInteraction::APointOfInteraction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);

	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	Sphere->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &APointOfInteraction::OnOverlapBegin);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &APointOfInteraction::OnOverlapEnd);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PointsText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PointsText"));
	PointsText->SetupAttachment(RootComponent);
	PointsText->SetHorizontalAlignment(EHTA_Center);
	PointsText->SetWorldSize(50.f);
	PointsText->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
}

// Called when the game starts or when spawned
void APointOfInteraction::BeginPlay()
{
	Super::BeginPlay();	

	PointsText->SetText(FText::FromString(FString::Printf(TEXT("+%d"), PointsValue)));
}

void APointOfInteraction::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	if (OverlappingPlayers.Contains(OtherActor)) return;

	ASKTBasePlayer* Player = Cast<ASKTBasePlayer>(OtherActor);
	if (Player)
	{
		Player->AddPoints(PointsValue);
		OverlappingPlayers.Add(OtherActor);
	}
}

void APointOfInteraction::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;

	OverlappingPlayers.Remove(OtherActor);
}
