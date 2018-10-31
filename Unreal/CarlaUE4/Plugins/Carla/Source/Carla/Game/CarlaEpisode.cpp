// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Game/CarlaEpisode.h"
#include "GameFramework/PlayerState.h"

#include "EngineUtils.h"
#include "GameFramework/SpectatorPawn.h"

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

TPair<EActorSpawnResultStatus, FActorView> UCarlaEpisode::SpawnActorWithInfo(
  const FTransform &Transform,
  FActorDescription ActorDescription)
{

  TArray<FString> MapKeys;
  TArray<FActorAttribute> MapValues;

  ActorDescription.Variations.GenerateValueArray(MapValues);
  ActorDescription.Variations.GenerateKeyArray(MapKeys);
  for (auto It = MapValues.CreateConstIterator(); It; ++It)
  {
    UE_LOG(LogCarla, Error, TEXT("MapValues '%s' (UId=%s)"), *It->Id, *It->Value);
  }
  for (auto It = MapKeys.CreateConstIterator(); It; ++It)
  {
    //Variations.Add(MapKeys[It.GetIndex()], MapValues[It.GetIndex()]);
    UE_LOG(LogCarla, Error, TEXT("MapKeys and values '%s' (UId=%s)"), *MapKeys[It.GetIndex()], *MapValues[It.GetIndex()].Value);
  }
  if (GetWorld()->IsServer()) {
    // We can spawn normally on the server
    return ActorDispatcher.SpawnActor(Transform, std::move(ActorDescription));
  }
  else {
    // We need to make an RPC call to the server
    ACarlaPlayerController *PlayerController = Cast<ACarlaPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));


    checkf(
      PlayerController != nullptr,
      TEXT("PlayerController is not a ACarlaPlayerController, did you forget to set it in the project settings?"));
    UE_LOG(LogCarla, Error, TEXT("Sending RPC with '%s' (UId=%d)"), *ActorDescription.Id, ActorDescription.UId);


    //AGameStateBase *GameState = UGameplayStatics::GetGameState(GetWorld());

    //checkf(
    //  GameState != nullptr,
    //  TEXT("GameState is null - this should not happen!"));
    APawn *pawn = PlayerController->GetPawnOrSpectator();
    APlayerState *state = pawn->PlayerState;


    uint32 IdOffset = state->PlayerId * 10000; // 10000 should be enough room for every client to spawn their own
    static IdType ID_COUNTER = 0u;
    const auto Id = ++ID_COUNTER + IdOffset;
    UE_LOG(LogCarla, Error, TEXT("PlayerState id (UId=%d)"), Id);
    PlayerController->SpawnActorWithInfo(Transform, std::move(ActorDescription));
    FActorView View = FActorView(Id, std::move(ActorDescription));
    return MakeTuple(EActorSpawnResultStatus::Success, View);
  }
}