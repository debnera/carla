// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Game/CarlaEpisode.h"
#include "GameFramework/PlayerState.h"

#include "Carla/Vehicle/VehicleSpawnPoint.h"

#include "EngineUtils.h"
#include "GameFramework/SpectatorPawn.h"

static FString UCarlaEpisode_GetTrafficSignId(ETrafficSignState State)
{
  using TSS = ETrafficSignState;
  switch (State) {
    case TSS::TrafficLightRed:
    case TSS::TrafficLightYellow:
    case TSS::TrafficLightGreen:  return TEXT("traffic.traffic_light");
    case TSS::SpeedLimit_30:      return TEXT("traffic.speed_limit.30");
    case TSS::SpeedLimit_40:      return TEXT("traffic.speed_limit.40");
    case TSS::SpeedLimit_50:      return TEXT("traffic.speed_limit.50");
    case TSS::SpeedLimit_60:      return TEXT("traffic.speed_limit.60");
    case TSS::SpeedLimit_90:      return TEXT("traffic.speed_limit.90");
    case TSS::SpeedLimit_100:     return TEXT("traffic.speed_limit.100");
    case TSS::SpeedLimit_120:     return TEXT("traffic.speed_limit.120");
    case TSS::SpeedLimit_130:     return TEXT("traffic.speed_limit.130");
    default:                      return TEXT("traffic.unknown");
  }
}

UCarlaEpisode::UCarlaEpisode(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer),
    Id([]() {
      static uint32 COUNTER = 0u;
      return ++COUNTER;
    }()) {}

TArray<FTransform> UCarlaEpisode::GetRecommendedStartTransforms() const
{
  TArray<FTransform> SpawnPoints;
  for (TActorIterator<AVehicleSpawnPoint> It(GetWorld()); It; ++It) {
    SpawnPoints.Add(It->GetActorTransform());
  }
  return SpawnPoints;
}

const AWorldObserver *UCarlaEpisode::StartWorldObserver(carla::streaming::MultiStream Stream)
{
  UE_LOG(LogCarla, Log, TEXT("Starting AWorldObserver sensor"));
  check(WorldObserver == nullptr);
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

  for (TActorIterator<ATrafficSignBase> It(World); It; ++It)
  {
    ATrafficSignBase *Actor = *It;
    check(Actor != nullptr);
    FActorDescription Description;
    Description.Id = UCarlaEpisode_GetTrafficSignId(Actor->GetTrafficSignState());
    Description.Class = Actor->GetClass();
    ActorDispatcher.GetActorRegistry().Register(*Actor, Description);
  }

  ACarlaPlayerController *Controller = Cast<ACarlaPlayerController>(PlayerController);
  if (!Controller) {
    UE_LOG(LogCarla, Error, TEXT("Missing CarlaPlayerController!"));
    return;
  }
  Controller->SetEpisode(this);
  SetPlayerController(Controller);
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
  if (ActorDescription.Id.Contains("sensor", ESearchCase::IgnoreCase) || GetWorld()->IsServer()) {
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
    static FActorView::IdType ID_COUNTER = 0u;
    const auto Id = ++ID_COUNTER + IdOffset;
    UE_LOG(LogCarla, Error, TEXT("PlayerState id (UId=%d)"), Id);
    ActorDescription.ActorReferenceId = Id;
    FActorView View = FActorView(Id, ActorDescription);
    PlayerController->SpawnActorWithInfo(Transform, std::move(ActorDescription));

    UE_LOG(LogCarla, Error, TEXT("Returning view with '%s' (UId=%d)"), *View.GetActorDescription()->Id, View.GetActorDescription()->UId);
    return MakeTuple(EActorSpawnResultStatus::Success, View);
  }
}