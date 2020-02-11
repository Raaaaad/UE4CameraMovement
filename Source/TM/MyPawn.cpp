// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "Components/InputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Camera/CameraComponent.h"
#include "MessageDialog.h"
#include "Engine.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CreateDefaultSubobject <UFloatingPawnMovement>("PawnMovement");

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	//show window with intructions
	FText Instruction = FText::FromString("Press Q to save start position.\nPress E to save next location. \nPress R to start replay.");
	const FText Title = FText::FromString("Instructions");
	FMessageDialog Instructions;
	Instructions.Debugf(Instruction, &Title);

	Super::BeginPlay();

	ReplayMovementAndRotation = false;
	Next = false;
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ReplayMovementAndRotation)
	{
		Replay();
	}
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMyPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyPawn::LookUp);
	
	PlayerInputComponent->BindAction("SaveStartLocation",IE_Pressed, this, &AMyPawn::SaveStartLocation);
	PlayerInputComponent->BindAction("SaveNext", IE_Pressed, this, &AMyPawn::SaveNext);
	PlayerInputComponent->BindAction("StartReplay", IE_Pressed, this, &AMyPawn::StartReplay);
}

//funtions responsible for movement and camera rotation
void AMyPawn::MoveForward(float Value)
{
	AddMovementInput(Camera->GetForwardVector(), Value);
}

void AMyPawn::MoveRight(float Value)
{
	AddMovementInput(Camera->GetRightVector(), Value);
}

void AMyPawn::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMyPawn::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMyPawn::SaveStartLocation()
{
	//make sure that there are no data in lists
	SavedLocations.clear();
	SavedRotations.clear();
	SavedLocations.push_front(GetActorLocation());
	SavedRotations.push_front(Camera->GetComponentRotation());
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Start location saved at: " + SavedLocations.front().ToString()
			+ " Start rotation saved at: " + SavedRotations.front().ToString());
}

void AMyPawn::SaveNext()
{
	//check if there was change in position or rotation
	if (GetActorLocation() != SavedLocations.front() || Camera->GetComponentRotation()!=SavedRotations.front()&&SavedLocations.size()>0)
	{
		SavedLocations.push_front(GetActorLocation());
		SavedRotations.push_front(Camera->GetComponentRotation());
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Next location saved at: " + SavedLocations.front().ToString()
				+ " Next rotation saved at: " + SavedRotations.front().ToString());
	}

}
void AMyPawn::StartReplay()
{
	if (SavedLocations.size() > 0 && SavedRotations.size() > 0)
	{
		//clear messages form screen
		if (GEngine)
			GEngine->ClearOnScreenDebugMessages();
		//check if position or rotation changed
		if (GetActorLocation() != SavedLocations.front() || Camera->GetComponentRotation() != SavedRotations.front())
		{
			SavedLocations.push_front(GetActorLocation());
			SavedRotations.push_front(Camera->GetComponentRotation());
		}
		
		//disable player input
		DisableInput(Cast<APlayerController>(this));

		//put the first location in variable
		StartLocation = SavedLocations.back();
		//destroy the first location
		SavedLocations.pop_back();
		//load next location
		EndLocation = SavedLocations.back();
		//destroy loaded location
		SavedLocations.pop_back();
		//set to to start position
		SetActorLocation(StartLocation);

		//same with camera rotation
		StartRotation = SavedRotations.back();
		SavedRotations.pop_back();
		EndRotation = SavedRotations.back();
		SavedRotations.pop_back();
		Camera->SetWorldRotation(StartRotation);

		//let the user know that replay is pending
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Replay pending..."));

		//start replay
		ReplayMovementAndRotation = true;
	}
}
void AMyPawn::Replay()
{
	if (!Next)
	{
		//variable for calculating current position during replay
		FVector NewLocation = GetActorLocation();
		//movement in X axis
		//value for synchronising movement and rotation, occures in every dimension
		float ValueX;
		//determining movement in suitable direction, if(EndLocation.X > StartLocation.X) then move forward
		if (EndLocation.X > StartLocation.X)
		{
			ValueX = (EndLocation.X - StartLocation.X) / 100;
			NewLocation.X += ValueX;
			//check for end of movement
			if (NewLocation.X >= EndLocation.X)Next = true;
			SetActorLocation(NewLocation);
		}
		//if (EndLocation.X > StartLocation.X) then move backward
		else if (EndLocation.X < StartLocation.X)
		{
			ValueX = (StartLocation.X - EndLocation.X) / 100;
			NewLocation.X -= ValueX;
			if (NewLocation.X <= EndLocation.X)Next = true;
			SetActorLocation(NewLocation);
		}
		//movement in Y axis
		float ValueY;
		if (EndLocation.Y > StartLocation.Y)
		{
			ValueY = (EndLocation.Y - StartLocation.Y) / 100;
			NewLocation.Y += ValueY;
			if (NewLocation.Y >= EndLocation.Y)Next = true;
			SetActorLocation(NewLocation);
		}
		else if (EndLocation.Y < StartLocation.Y)
		{
			ValueY = (StartLocation.Y - EndLocation.Y) / 100;
			NewLocation.Y -= ValueY;
			if (NewLocation.Y <= EndLocation.Y)Next = true;
			SetActorLocation(NewLocation);
		}
		//movement in Z axis
		float ValueZ;
		if (EndLocation.Z > StartLocation.Z)
		{
			ValueZ = (EndLocation.Z - StartLocation.Z) / 100;
			NewLocation.Z += ValueZ;
			if (NewLocation.Z >= EndLocation.Z)Next = true;
			SetActorLocation(NewLocation);
		}
		else if (EndLocation.Z < StartLocation.Z)
		{
			ValueZ = (StartLocation.Z - EndLocation.Z) / 100;
			NewLocation.Z -= ValueZ;
			if (NewLocation.Z <= EndLocation.Z)Next = true;
			SetActorLocation(NewLocation);
		}
		//camera rotation
		FRotator NewRotation = Camera->GetComponentRotation();
		//yaw rotation
		float ValueYaw;
		if (EndRotation.Yaw > StartRotation.Yaw)
		{
			ValueYaw = (EndRotation.Yaw - StartRotation.Yaw) / 100.f;
			NewRotation.Yaw += ValueYaw;
			if (NewRotation.Yaw >= EndRotation.Yaw)Next = true;
			Camera->SetWorldRotation(NewRotation);
		}
		else if (EndRotation.Yaw < StartRotation.Yaw)
		{
			ValueYaw = (StartRotation.Yaw - EndRotation.Yaw) / 100.f;
			NewRotation.Yaw -= ValueYaw;
			if (NewRotation.Yaw <= EndRotation.Yaw)Next = true;
			Camera->SetWorldRotation(NewRotation);
		}
		//pitch rotation
		float ValuePitch;
		if (EndRotation.Pitch > StartRotation.Pitch)
		{
			ValuePitch = (EndRotation.Pitch - StartRotation.Pitch) / 100.f;
			NewRotation.Pitch += ValuePitch;
			if (NewRotation.Pitch >= EndRotation.Pitch)Next = true;
			Camera->SetWorldRotation(NewRotation);
		}
		else if (EndRotation.Pitch < StartRotation.Pitch)
		{
			ValuePitch = (StartRotation.Pitch - EndRotation.Pitch) / 100.f;
			NewRotation.Pitch -= ValuePitch;
			if (NewRotation.Pitch <= EndRotation.Pitch)Next = true;
			Camera->SetWorldRotation(NewRotation);
		}		
	}
	//load next location
	else if (Next && SavedLocations.size() > 0 && SavedLocations.size()>0)
	{
		StartLocation = EndLocation;
		EndLocation = SavedLocations.back();
		SavedLocations.pop_back();

		StartRotation = EndRotation;
		EndRotation = SavedRotations.back();
		SavedRotations.pop_back();

		Next = false;
	}
	//end of a replay
	else if (SavedLocations.size() <= 0 || SavedRotations.size()<=0)
	{
		//clear messages form the screen
		if (GEngine)
			GEngine->ClearOnScreenDebugMessages();
		//make sure that there are no data left in the lists
		SavedLocations.clear();
		SavedRotations.clear();
		//enable user input
		EnableInput(Cast<APlayerController>(this));
		//stop replay
		ReplayMovementAndRotation = false;
		//prepare for a next replay, so it won't the load a location immediately
		Next = false;
	}
}


