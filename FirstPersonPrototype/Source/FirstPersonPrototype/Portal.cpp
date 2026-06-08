// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "FirstPersonPrototypeCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Default Sub Objects to Initalize Componenets.
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	boxComp = CreateDefaultSubobject<UBoxComponent>("Box Comp");
	sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");

	RootComponent = boxComp;
	mesh->SetupAttachment(boxComp);
	sceneCapture->SetupAttachment(mesh);

	mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverLapBegin);
	mesh->SetHiddenInSceneCapture(true);


	// Checks if material is valid. 
	if (mat) {
		mesh->SetMaterial(0, mat);
	}
	
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePortals();

}

void APortal::OnOverLapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFirstPersonPrototypeCharacter* playerChar = Cast<AFirstPersonPrototypeCharacter>(OtherActor);

	if (playerChar)
	{
		if (OtherPortal)
		{
			if (!playerChar->isTeleporting)
			{
				playerChar->isTeleporting = true;
				FVector loc = OtherPortal->GetActorLocation();
				playerChar->SetActorLocation(loc);

				FTimerHandle TimerHandle; 
				FTimerDelegate TimerDelegate; 
				TimerDelegate.BindUFunction(this, "SetBool", playerChar);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1, false);


			}
		}
	}
}

void APortal::SetBool(AFirstPersonPrototypeCharacter* playerChar)
{
if (playerChar)
{
	playerChar->isTeleporting = false;
}
}



void APortal::UpdatePortals()
{
	FVector Location = this->GetActorLocation() - OtherPortal->GetActorLocation();
	FVector camLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentLocation();
	FRotator camRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentRotation();
	FVector CombinedLocation = camLocation + Location; 
}

