// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Game/CarlaEpisode.h"

#include "EngineUtils.h"
#include "GameFramework/SpectatorPawn.h"

UCarlaEpisode::UCarlaEpisode(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer),
    Id([]() {
      static uint32 COUNTER = 0u;
      return ++COUNTER;
    }()) {}

const AWorldObserver *UCarlaEpisode::StartWorldObserver(carla::streaming::MultiStream Stream)
{
  UE_LOG(LogCarla, Log, TEXT("Starting AWorldObserver sensor"));
  check(WorldObserver != nullptr);
  auto *World = GetWorld();
  check(World != nullptr);
  WorldObserver = World->SpawnActorDeferred<AWorldObserver>(
      AWorldObserver::StaticClass(),
      FTransform(),
      nullptr,
      nullptr,
      ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
  WorldObserver->SetEpisode(*this);
  WorldObserver->SetStream(std::move(Stream));
  UGameplayStatics::FinishSpawningActor(WorldObserver, FTransform());
  return WorldObserver;
}

void UCarlaEpisode::InitializeAtBeginPlay()
{
  auto World = GetWorld();
  check(World != nullptr);
  auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
  if (PlayerController == nullptr)
  {
    UE_LOG(LogCarla, Error, TEXT("Can't find player controller!"));
    return;
  }
  Spectator = PlayerController->GetPawn();
  if (Spectator != nullptr)
  {
    FActorDescription Description;
    Description.Id = TEXT("spectator");
    Description.Class = Spectator->GetClass();
    ActorDispatcher.GetActorRegistry().Register(*Spectator, Description);
  }
  else
  {
    UE_LOG(LogCarla, Error, TEXT("Can't find spectator!"));
  }
}
