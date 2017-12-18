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
	if (PressurePlate == nullptr) 
		UE_LOG(LogTemp, Error, TEXT("Missing TriggerVolume on %s"), *GetOwner()->GetName());
}

void UOpenDoor::OpenDoor()
{
	OnOpen.Broadcast();
}

void UOpenDoor::CloseDoor()
{
	OnClose.Broadcast();
}


void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PressurePlate) {return;}
	if (GetTotalMassOfActorsOnPlate() > MassToOpen) {
		OpenDoor();
	}
	else CloseDoor();
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.f;
	//Find all overlapping Actors
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) return 0.f;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	// iterate and add all masses
	for (const auto& actor : OverlappingActors)
		TotalMass += actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	return TotalMass;
}