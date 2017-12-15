// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Gameframework/Actor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

UOpenDoor::UOpenDoor()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();	
}

void UOpenDoor::OpenDoor()
{
	Owner->SetActorRotation(FRotator(0.0f, OpenAngle, 0.0f));
}

void UOpenDoor::CloseDoor()
{
	Owner->SetActorRotation(FRotator(0.0f, 0.f, 0.0f));
}


void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetTotalMassOfActorsOnPlate() > 50.f) {
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}
	//check if time to close the door
	else 
		if(GetWorld()->GetTimeSeconds() - LastDoorOpenTime > CloseDelay)
			CloseDoor();
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.f;
	//Find all overlapping Actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	// iterate and add all masses
	for (const auto& actor : OverlappingActors)
	{
		TotalMass += actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("mass: %f"), TotalMass);
	}
	
	return TotalMass;
}