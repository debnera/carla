// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "Carla/Actor/CarlaActorFactory.h"
#include "Carla/Game/CarlaEpisode.h"

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CarlaGameStateBase.generated.h"

class UTaggerDelegate;

UCLASS()
class CARLA_API ACarlaGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	//UPROPERTY(VisibleAnywhere)
	//TArray<FString> ServerAdresses;

	ACarlaGameStateBase(const FObjectInitializer &ObjectInitializer);

	void ReceiveEndPlay(EEndPlayReason::Type EndPlayReason);

	void Tick(float DeltaSeconds) override;

protected:

	void OnRep_ReplicatedHasBegunPlay() override;

	//UFUNCTION(Server)
	//bool TryAddServerAddress(FString address);


private:

	void SpawnActorFactories();

	bool bServerIsRunning = false;

	UPROPERTY()
	UCarlaGameInstance *GameInstance = nullptr;

	UPROPERTY()
	UTaggerDelegate *TaggerDelegate = nullptr;

	UPROPERTY()
	UCarlaEpisode *Episode = nullptr;

	/// List of actor spawners that will be used to define and spawn the actors
	/// available in game.
	UPROPERTY(EditAnywhere)
	TSet<TSubclassOf<ACarlaActorFactory>> ActorFactories;

	UPROPERTY()
	TArray<ACarlaActorFactory *> ActorFactoryInstances;

};
