// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Carla/Actor/ActorDescription.h"
#include "CarlaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CARLA_API ACarlaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
  UFUNCTION( Server, Reliable, WithValidation )
  void SpawnActorWithInfo(const FTransform &Transform,
                          FActorDescription ActorDescription);
	
	
};
