// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "PointOfInteraction.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class PROSKATER_API APointOfInteraction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APointOfInteraction();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectable")
	int32 PointsValue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTextRenderComponent* PointsText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collectable")
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	// Armazena os players que estão dentro da esfera, evitando múltiplos triggers
	UPROPERTY()
	TSet<AActor*> OverlappingPlayers;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
