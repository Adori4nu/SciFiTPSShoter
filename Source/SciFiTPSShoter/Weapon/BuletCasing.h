// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuletCasing.generated.h"

UCLASS()
class SCIFITPSSHOTER_API ABuletCasing : public AActor
{
	GENERATED_BODY()
	
public:
	ABuletCasing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulseValue;

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

	FTimerHandle DestroyTimerHandle;
};
