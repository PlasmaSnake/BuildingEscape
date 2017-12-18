// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT
UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
}

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
	if (PhysicsHandle==nullptr) UE_LOG(LogTemp, Error, TEXT("Physics Handler not found on %s"), *ownerName);
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
	return hit;
}

void UGrabber::Grab(){
	
	///LINE TRACE to See if any actors are reachable with physics body collision channel set
	SetLineTraceAndPlayerViewPoint();
	auto hitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = hitResult.GetComponent();
	auto actorHit = hitResult.GetActor();
	//If Hit then attach physics handle
	if (actorHit) {
		if (!PhysicsHandle) {return;}
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true
		);
	}

}

void UGrabber::Release() {
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

void UGrabber::SetLineTraceAndPlayerViewPoint() {
	///Sets player View Point
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT playerViewPointLocation,
		OUT playerViewPointRotation);
	///Sets Line Trace
	LineTraceEnd = playerViewPointLocation + playerViewPointRotation.Vector() * Reach;
}

void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// if physics handle is attached otherwise break Tick-loop
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent){	
		//move the object held
		SetLineTraceAndPlayerViewPoint();
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}