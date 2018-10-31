// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "Carla/Actor/ActorDispatcher.h"
#include "Kismet/GameplayStatics.h"
#include "Carla/Game/CarlaPlayerController.h"

#include "CarlaEpisode.generated.h"

/// A simulation episode.
///
/// Each time the level is restarted a new episode is created.
UCLASS(BlueprintType, Blueprintable)
class CARLA_API UCarlaEpisode : public UObject
{
  GENERATED_BODY()

public:

  void SetMapName(const FString &InMapName)
  {
    MapName = InMapName;
  }

  void InitializeAtBeginPlay();

  UFUNCTION(BlueprintCallable)
  const FString &GetMapName() const
  {
    return MapName;
  }

  UFUNCTION(BlueprintCallable)
  APawn *GetSpectatorPawn() const
  {
    return Spectator;
  }

  void RegisterActorFactory(ACarlaActorFactory &ActorFactory)
  {
    ActorDispatcher.Bind(ActorFactory);
  }

  /// Return the list of actor definitions that are available to be spawned this
  /// episode.
  UFUNCTION(BlueprintCallable)
  const TArray<FActorDefinition> &GetActorDefinitions() const
  {
    return ActorDispatcher.GetActorDefinitions();
  }

  /// Spawns an actor based on @a ActorDescription at @a Transform. To properly
  /// despawn an actor created with this function call DestroyActor.
  ///
  /// @return A pair containing the result of the spawn function and a view over
  /// the actor and its properties. If the status is different of Success the
  /// view is invalid.
  TPair<EActorSpawnResultStatus, FActorView> SpawnActorWithInfo(
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


      PlayerController->SpawnActorWithInfo(Transform, std::move(ActorDescription));
      uint32 Id = 1;
      FActorView View = FActorView(Id, std::move(ActorDescription));
      return MakeTuple(EActorSpawnResultStatus::Success, View);
    }
  }

  /// Spawns an actor based on @a ActorDescription at @a Transform. To properly
  /// despawn an actor created with this function call DestroyActor.
  ///
  /// @return nullptr on failure.
  ///
  /// @note Special overload for blueprints.
  UFUNCTION(BlueprintCallable)
  AActor *SpawnActor(
      const FTransform &Transform,
      FActorDescription ActorDescription)
  {
    return SpawnActorWithInfo(Transform, std::move(ActorDescription)).Value.GetActor();
  }

  /// @copydoc FActorDispatcher::DestroyActor(AActor*)
  UFUNCTION(BlueprintCallable)
  bool DestroyActor(AActor *Actor)
  {
    return ActorDispatcher.DestroyActor(Actor);
  }

  const FActorRegistry &GetActorRegistry() const
  {
    return ActorDispatcher.GetActorRegistry();
  }

private:

  UPROPERTY(VisibleAnywhere)
  FString MapName;

  FActorDispatcher ActorDispatcher;

  UPROPERTY(VisibleAnywhere)
  APawn *Spectator = nullptr;
};
