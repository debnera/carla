// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.


#include "Carla.h"
#include "CarlaPlayerController.h"
#include "CarlaGameInstance.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Vehicle/MyWheeledVehicleAIController.h"


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
  //UE_LOG(LogCarla, Error, TEXT("Spawning with '%s' (UId=%d)"), *ActorDescription.Id, ActorDescription.UId);
  Episode->SpawnActorWithInfo(Transform, std::move(ActorDescription));

}


void ACarlaPlayerController::ApplyControlToActor_Implementation(uint32 ActorId, FVehicleControl Control)
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
  /*
  if (!GetEpisode()) {
    UE_LOG(LogCarla, Error, TEXT("CarlaPlayerController is missing CarlaEpisode!"));
    return;
  }*/
  auto ActorView = Episode->GetActorRegistry().Find(ActorId);
  if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill()) {
    //RespondErrorStr("unable to apply control: actor not found");
    return;
  }
  auto Vehicle = Cast<ACarlaWheeledVehicle>(ActorView.GetActor());
  if (Vehicle == nullptr) {
    //RespondErrorStr("unable to apply control: actor is not a vehicle");
    return;
  }
  Vehicle->ApplyVehicleControl(Control);
}


void ACarlaPlayerController::SetActorAutopilot_Implementation(uint32 ActorId, bool bEnabled)
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
  /*
  if (!GetEpisode()) {
    UE_LOG(LogCarla, Error, TEXT("CarlaPlayerController is missing CarlaEpisode!"));
    return;
  }*/
  auto ActorView = Episode->GetActorRegistry().Find(ActorId);
  if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill()) {
    //RespondErrorStr("unable to set autopilot: actor not found");
    return;
  }
  auto Vehicle = Cast<ACarlaWheeledVehicle>(ActorView.GetActor());
  if (Vehicle == nullptr) {
    //RespondErrorStr("unable to set autopilot: actor is not a vehicle");
    return;
  }
  auto Controller = Cast<AWheeledVehicleAIController>(Vehicle->GetController());
  auto Controller2 = Cast<AMyWheeledVehicleAIController>(Vehicle->GetController());
  if (Vehicle->GetController() == nullptr) {
    //RespondErrorStr("unable to set autopilot: vehicle does not have a controller");
    return;
  }
  if (Controller != nullptr) {
    Controller->SetAutopilot(bEnabled);
  }
  if (Controller2 != nullptr) {
    Controller2->SetAutopilot(bEnabled);
  }
  //RespondErrorStr("unable to set autopilot: vehicle has an incompatible controller");
}


bool ACarlaPlayerController::SpawnActorWithInfo_Validate(
  const FTransform &Transform,
  FActorDescription ActorDescription)
{
  return true;
}

bool ACarlaPlayerController::ApplyControlToActor_Validate(uint32 ActorId, FVehicleControl Control)
{
  return true;
}

bool ACarlaPlayerController::SetActorAutopilot_Validate(uint32 ActorId, bool bEnabled)
{
  return true;
}

