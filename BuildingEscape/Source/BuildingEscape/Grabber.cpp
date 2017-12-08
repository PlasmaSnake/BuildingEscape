// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT
// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Grabber instantiated."));
	
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get player viewpoint
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT playerViewPointLocation, 
		OUT playerViewPointRotation);
	/// TODO Player View Testing UE_LOG(LogTemp, Warning, TEXT("Location: %s Rotation: %s"), *playerViewPointLocation.ToString(), *playerViewPointRotation.ToString());*/
	FVector LineTraceEnd = playerViewPointLocation + playerViewPointRotation.Vector() * Reach;
	///Draw a red line for visualization
	DrawDebugLine(
		GetWorld(),
		playerViewPointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.f);

	///Ray-cast out to reach distance
	FHitResult hit;
	FCollisionQueryParams traceParameters(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT hit,
		playerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		traceParameters
	);
	AActor *actorHit = hit.GetActor();
	if (actorHit) UE_LOG(LogTemp, Warning, TEXT("%s is grabbable"), *(actorHit->GetName()));

	/// See if item is grabable
}

