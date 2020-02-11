// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "list"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

UCLASS()
class TM_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();
	//object definition
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* CubeMesh;
	//camera definition
	UPROPERTY(EditAnywhere)
		class UCameraComponent* Camera;
	//variables for determining start location and rotation	
	FVector StartLocation;
	FRotator StartRotation;
	//variables for determining end location and rotation
	FVector EndLocation;
	FRotator EndRotation;

	
	std::list<FVector> SavedLocations;
	std::list<FRotator> SavedRotations;

	//variable for starting replay
	bool ReplayMovementAndRotation;
	//variable to see if next position should be loaded
	bool Next;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void SaveStartLocation();
	void SaveNext();
	void StartReplay();
	void Replay();
};
