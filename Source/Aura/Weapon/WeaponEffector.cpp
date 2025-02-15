// Copyright Druid Mechanics


#include "WeaponEffector.h"

#include "WeaponTypes.h"
#include "Aura/Components/CombatComponent.h"
#include "Character/ShooterCharacter.h"


void AWeaponEffector::OnOverlap(AActor* TargetActor)
{
	 AShooterCharacter* Character = Cast<AShooterCharacter>(TargetActor);
	 if (Character != nullptr)
	 {
		 switch (WeaponType)
		 {
		 case EWeaponType::EWT_AssaultRifle:
		 	Character->Combat->CarriedAmmoMap[WeaponType] += 30;
		 	Character->Combat->UpdateAmmoValues();
		 	break;
		 case EWeaponType::EWT_Shotgun:
		 	Character->Combat->CarriedAmmoMap[WeaponType] += 30;
		 	Character->Combat->UpdateAmmoValues();
		 	break;
		 case EWeaponType::EWT_RocketLauncher:
		 	Character->Combat->CarriedAmmoMap[WeaponType] += 5;
		 	Character->Combat->UpdateAmmoValues();
		 	break;
		 case EWeaponType::EWT_Pistol:
		 	Character->Combat->CarriedAmmoMap[WeaponType] += 4;
		 	Character->Combat->UpdateAmmoValues();
		 	break;
		 }
	}
	Destroy();
}
