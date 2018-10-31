// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "CarlaPlayerController.h"
#include "CarlaGameInstance.h"
#include "Carla/Game/CarlaEpisode.h"


void ACarlaPlayerController::SpawnActorWithInfo_Implementation(
    const FTransform &Transform,
    FActorDescription ActorDescription)
{
  UCarlaGameInstance *GameInstance = Cast<UCarlaGameInstance>(GetGameInstance());
  checkf(
      GameInstance != nullptr,
      TEXT("GameInstance is not a UCarlaGameInstance, did you forget to set it in the project settings?"));
  UCarlaEpisode *Episode = GameInstance->GetEpisode();
  if (Episode == nullptr) {
    UE_LOG(LogCarla, Log, TEXT("Cannot spawn actor - server is missing Episode!"));
    return;
  }
  UE_LOG(LogCarla, Error, TEXT("Spawning with '%s' (UId=%d)"), *ActorDescription.Id, ActorDescription.UId);
  Episode->SpawnActorWithInfo(Transform, std::move(ActorDescription));

}


bool ACarlaPlayerController::SpawnActorWithInfo_Validate(
    const FTransform &Transform,
    FActorDescription ActorDescription)
{
  // We don't have to worry about the client trying to cheat, thus we can always allow this call
  return true;
}