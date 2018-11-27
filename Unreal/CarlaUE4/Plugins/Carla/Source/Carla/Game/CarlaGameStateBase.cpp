// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "CarlaGameStateBase.h"

#include "Carla/Game/CarlaGameInstance.h"
#include "Carla/Game/Tagger.h"
#include "Carla/Game/TaggerDelegate.h"

#include "Engine/GameEngine.h"

ACarlaGameStateBase::ACarlaGameStateBase(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer) {
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickGroup = TG_PrePhysics;
  bAllowTickBeforeBeginPlay = false;

  bServerIsRunning = false;

  Episode = CreateDefaultSubobject<UCarlaEpisode>(TEXT("Episode"));

  TaggerDelegate = CreateDefaultSubobject<UTaggerDelegate>(TEXT("TaggerDelegate"));
  if(GEngine)
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Constructed!"));
}



void ACarlaGameStateBase::OnRep_ReplicatedHasBegunPlay() {
  AGameStateBase::OnRep_ReplicatedHasBegunPlay();

  // ------- InitGame() from TheNewCarlaGameModeBase -------
  checkf(
      Episode != nullptr,
      TEXT("Missing episode, can't continue without an episode!"));
  //Episode->SetMapName("Map_name"); /// @todo Get map name from somewhere

  GameInstance = Cast<UCarlaGameInstance>(GetGameInstance());
  checkf(
      GameInstance != nullptr,
      TEXT("GameInstance is not a UCarlaGameInstance, did you forget to set it in the project settings?"));

  if (TaggerDelegate != nullptr) {
    check(GetWorld() != nullptr);
    TaggerDelegate->RegisterSpawnHandler(GetWorld());
  } else {
    UE_LOG(LogCarla, Error, TEXT("Missing TaggerDelegate!"));
  }

  SpawnActorFactories();
  if(GEngine)
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("InitGamed!"));

  // ------- BeginPlay() from TheNewCarlaGameModeBase -------
  if (true) { /// @todo If semantic segmentation enabled.
    check(GetWorld() != nullptr);
    ATagger::TagActorsInLevel(*GetWorld(), true);
    TaggerDelegate->SetSemanticSegmentationEnabled();
  }

  Episode->InitializeAtBeginPlay();
  bServerIsRunning = true;
  GameInstance->NotifyBeginEpisode(*Episode);
  if(GEngine)
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("BeginPlayed!"));
}

void ACarlaGameStateBase::ReceiveEndPlay(EEndPlayReason::Type EndPlayReason) {
  GameInstance->NotifyEndEpisode();

  if(GEngine)
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("EndPlay!"));
  AActor::ReceiveEndPlay(EndPlayReason);
}

void ACarlaGameStateBase::Tick(float DeltaSeconds) {
  AActor::Tick(DeltaSeconds);

  if (bServerIsRunning)
    GameInstance->Tick(DeltaSeconds);
  //if(GEngine)
  //  GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Tick!"));
}

/*
bool ACarlaGameStateBase::TryAddServerAddress(FString address) {
  if (ServerAdresses.Contains(address))
    return false;
  ServerAdresses.Add(address);
  return true;
}*/

void ACarlaGameStateBase::SpawnActorFactories()
{
  auto *World = GetWorld();
  check(World != nullptr);

  for (auto &FactoryClass : ActorFactories)
  {
    if (FactoryClass != nullptr)
    {
      auto *Factory = World->SpawnActor<ACarlaActorFactory>(FactoryClass);
      if (Factory != nullptr)
      {
        Episode->RegisterActorFactory(*Factory);
        ActorFactoryInstances.Add(Factory);
      }
      else
      {
        UE_LOG(LogCarla, Error, TEXT("Failed to spawn actor spawner"));
      }
    }
  }

}