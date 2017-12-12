// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT
// Sets default values for this component's properties
UGrabber::UGrabber()
{

	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandlerComponent();
	SetupInputComponent();
}

void UGrabber::FindPhysicsHandlerComponent()
{
	FString ownerName = GetOwner()->GetName();
	// Look for attached Physics Handle Component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		//PhysicsHandle is found
	}
	else UE_LOG(LogTemp, Error, TEXT("Physics Handler not found on %s"), *ownerName);
}

void UGrabber::SetupInputComponent()
{
	FString ownerName = GetOwner()->GetName();
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		/// Bind Input Axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else UE_LOG(LogTemp, Error, TEXT("Input Component not found on %s"), *ownerName);
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	/// Get player viewpoint
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT playerViewPointLocation,
		OUT playerViewPointRotation);
	/// TODO Player View Testing UE_LOG(LogTemp, Warning, TEXT("Location: %s Rotation: %s"), *playerViewPointLocation.ToString(), *playerViewPointRotation.ToString());*/
	FVector LineTraceEnd = playerViewPointLocation + playerViewPointRotation.Vector() * Reach;
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
	if (actorHit) UE_LOG(LogTemp, Warning, TEXT("Line Trace Detected: %s"), *(actorHit->GetName()));
	return hit;
}

void UGrabber::Grab(){
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"));
	///LINE TRACE to See if any actors are reachable with physics body collision channel set
	auto hitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = hitResult.GetComponent();
	auto actorHit = hitResult.GetActor();

	//If Hit then attach physics handle
	if(actorHit)
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true
		);

}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"));
	//TODO Release Physics handle
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// Get player viewpoint
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT playerViewPointLocation,
		OUT playerViewPointRotation);
	/// TODO Player View Testing UE_LOG(LogTemp, Warning, TEXT("Location: %s Rotation: %s"), *playerViewPointLocation.ToString(), *playerViewPointRotation.ToString());*/
	FVector LineTraceEnd = playerViewPointLocation + playerViewPointRotation.Vector() * Reach;

	// if physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
		//move the object held
		PhysicsHandle->SetTargetLocation(LineTraceEnd);

}

