// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class AURA_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle SpecHandle = nullptr;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;
	class UParticleSystemComponent* TracerComponent;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	
};

