// Copyright Druid Mechanics


#include "ProjectileWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// From muzzle flash socket to hit location from TraceUnderCrosshairs
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass && InstigatorPawn)
		{
			AActor* OwnerActor = GetOwner();

			const FTransform SpawnTransform(TargetRotation, SocketTransform.GetLocation());

			if (UWorld* World = GetWorld())
			{
				AProjectile* NewProjectile = World->SpawnActorDeferred<AProjectile>(
					ProjectileClass,
					SpawnTransform,       // Full spawn transform (location, rotation, scale)
					OwnerActor,           // Owner
					InstigatorPawn	     // Instigator
				);

				if (NewProjectile)
				{
					NewProjectile->FinishSpawning(SpawnTransform);
				}
			}
		}
	}
}
