// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "FirstPersonPrototypeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Default Sub Objects to Initalize Componenets.
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	boxComp = CreateDefaultSubobject<UBoxComponent>("Box Comp");
	sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");
	PlayerDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("PlayerDirectionArrow"));

	RootComponent = boxComp;
	mesh->SetupAttachment(boxComp);
	sceneCapture->SetupAttachment(mesh);

	mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (RootComponent)
	{
		PlayerDirection->SetupAttachment(RootComponent);
	}


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

#include "Engine/Engine.h" //GEngine

void APortal::OnOverLapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFirstPersonPrototypeCharacter* playerChar = Cast<AFirstPersonPrototypeCharacter>(OtherActor);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Portal Teleport Activated"));
	}

	

	if (playerChar)
	{
		if (OtherPortal)
		{
			if (!playerChar->isTeleporting)
			{
				playerChar->isTeleporting = true;
				FVector loc = OtherPortal->GetActorLocation();
				playerChar->SetActorLocation(loc);

				// new 
				//FRotator rot = OtherPortal->GetActorRotation();
				//playerChar->SetActorRotation(rot);
				

				// 2. ACTIVATE THE ROTATION (We call our custom function using the destination portal's layout)
				OtherPortal->RotatePlayerToArrow(playerChar);

				//if (PlayerDirection && playerChar) 
				//{
				//	FRotator TargetRotation = PlayerDirection->GetComponentRotation();
				//	playerChar->SetActorRotation(TargetRotation);
				//}


				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, rot.ToString());

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

void APortal::RotatePlayerToArrow(ACharacter* playerChar)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("3. Inside RotatePlayerToArrow Function!"));
	}

	if (!PlayerDirection)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: PlayerDirection Arrow Component is NULL!"));
		}
		return;
	}

	if (!playerChar)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: playerChar is NULL inside rotation function!"));
		}
		return;
	}

	// If everything passed, execute rotation
	FRotator TargetRotation = PlayerDirection->GetComponentRotation();
	playerChar->SetActorRotation(TargetRotation);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetControlRotation(TargetRotation);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("SUCCESS: Camera Snapped to: %s"), *TargetRotation.ToString()));
		}
	}
}

void APortal::UpdatePortals()
{
	FVector Location = this->GetActorLocation() - OtherPortal->GetActorLocation();
	FVector camLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentLocation();
	FRotator camRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentRotation();
	FVector CombinedLocation = camLocation + Location; 
}

