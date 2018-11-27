// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Actor/ActorDescription.h"
#include "Carla/Game/CarlaGameInstance.h"
#include "Carla/Game/CarlaEpisode.h"
#include "CarlaWheeledVehicle.h"

#include "Agent/VehicleAgentComponent.h"
#include "WheeledVehicleAIController.h"

#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
#include "UnrealNetwork.h"

// =============================================================================
// -- Constructor and destructor -----------------------------------------------
// =============================================================================

ACarlaWheeledVehicle::ACarlaWheeledVehicle(const FObjectInitializer& ObjectInitializer) :
  Super(ObjectInitializer)
{
  VehicleBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("VehicleBounds"));
  VehicleBounds->SetupAttachment(RootComponent);
  VehicleBounds->SetHiddenInGame(true);
  VehicleBounds->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

  VehicleAgentComponent = CreateDefaultSubobject<UVehicleAgentComponent>(TEXT("VehicleAgentComponent"));
  VehicleAgentComponent->SetupAttachment(RootComponent);

  if (GetWorld() && GetWorld()->IsServer()) {

    //AIControllerClass = AWheeledVehicleAIController::StaticClass();
    //SpawnDefaultController();
  }

  GetVehicleMovementComponent()->bReverseAsBrake = false;
}

ACarlaWheeledVehicle::~ACarlaWheeledVehicle() {}

// =============================================================================
// -- Get functions ------------------------------------------------------------
// =============================================================================

float ACarlaWheeledVehicle::GetVehicleForwardSpeed() const
{
  return GetVehicleMovementComponent()->GetForwardSpeed();
}

FVector ACarlaWheeledVehicle::GetVehicleOrientation() const
{
  return GetVehicleTransform().GetRotation().GetForwardVector();
}

int32 ACarlaWheeledVehicle::GetVehicleCurrentGear() const
{
  return GetVehicleMovementComponent()->GetCurrentGear();
}

FTransform ACarlaWheeledVehicle::GetVehicleBoundingBoxTransform() const
{
  return VehicleBounds->GetRelativeTransform();
}

FVector ACarlaWheeledVehicle::GetVehicleBoundingBoxExtent() const
{
  return VehicleBounds->GetScaledBoxExtent();
}

float ACarlaWheeledVehicle::GetMaximumSteerAngle() const
{
  const auto &Wheels = GetVehicleMovementComponent()->Wheels;
  check(Wheels.Num() > 0);
  const auto *FrontWheel = Wheels[0];
  check(FrontWheel != nullptr);
  return FrontWheel->SteerAngle;
}

// =============================================================================
// -- Set functions ------------------------------------------------------------
// =============================================================================

void ACarlaWheeledVehicle::ApplyVehicleControl(const FVehicleControl &VehicleControl)
{
  SetThrottleInput(VehicleControl.Throttle);
  SetSteeringInput(VehicleControl.Steer);
  SetBrakeInput(VehicleControl.Brake);
  SetHandbrakeInput(VehicleControl.bHandBrake);
  SetReverse(VehicleControl.bReverse);
}

void ACarlaWheeledVehicle::SetThrottleInput(const float Value)
{
  GetVehicleMovementComponent()->SetThrottleInput(Value);
  Control.Throttle = Value;
}

void ACarlaWheeledVehicle::SetSteeringInput(const float Value)
{
  GetVehicleMovementComponent()->SetSteeringInput(Value);
  Control.Steer = Value;
}

void ACarlaWheeledVehicle::SetBrakeInput(const float Value)
{
  GetVehicleMovementComponent()->SetBrakeInput(Value);
  Control.Brake = Value;
}

void ACarlaWheeledVehicle::SetReverse(const bool Value)
{
  if (Value != Control.bReverse) {
    Control.bReverse = Value;
    auto MovementComponent = GetVehicleMovementComponent();
    MovementComponent->SetUseAutoGears(!Control.bReverse);
    MovementComponent->SetTargetGear(Control.bReverse ? -1 : 1, true);
  }
}

void ACarlaWheeledVehicle::SetHandbrakeInput(const bool Value)
{
  GetVehicleMovementComponent()->SetHandbrakeInput(Value);
  Control.bHandBrake = Value;
}

void ACarlaWheeledVehicle::OnRep_Description()
{
  UCarlaGameInstance *GameInstance = Cast<UCarlaGameInstance>(GetWorld()->GetGameInstance());
  checkf(
    GameInstance != nullptr,
    TEXT("GameInstance is not a UCarlaGameInstance, did you forget to set it in the project settings?"));
  UCarlaEpisode *Episode = GameInstance->GetEpisode();
  if (Episode == nullptr) {
    UE_LOG(LogCarla, Log, TEXT("Cannot spawn actor - server is missing Episode!"));
    return;
  }
  Episode->RegisterActor(this, Description);

}

void ACarlaWheeledVehicle::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);
  DOREPLIFETIME(ACarlaWheeledVehicle, Description);
}