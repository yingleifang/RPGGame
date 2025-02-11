// Copyright Druid Mechanics


#include "ProjectileWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

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
			
			FTransform SpawnTransform(TargetRotation, SocketTransform.GetLocation());

			UWorld* World = GetWorld();
			if (World)
			{
				AProjectile* NewProjectile = World->SpawnActorDeferred<AProjectile>(
					ProjectileClass,
					SpawnTransform,       // Full spawn transform (location, rotation, scale)
					OwnerActor,           // Owner
					InstigatorPawn	     // Instigator
				);

				if (NewProjectile)
				{
					NewProjectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

					NewProjectile->FinishSpawning(SpawnTransform);
				}
			}
		}
	}
}

