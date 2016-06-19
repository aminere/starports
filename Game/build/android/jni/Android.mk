
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := starport
LOCAL_LDLIBS    := -llog -landroid -lGLESv2 -lEGL
LOCAL_STATIC_LIBRARIES := shoot

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../../../src/game

ifeq ($(SHOOT_PROFILE), 1)
LOCAL_CFLAGS += -D_PROFILE_
endif

ifeq ($(SHOOT_RETAIL), 1)
LOCAL_CFLAGS += -D_RETAIL_
endif

LOCAL_SRC_FILES := \
main.cpp \
../../../src/game/Actor.cpp \
../../../src/game/AIManager.cpp \
../../../src/game/AttackerSettings.cpp \
../../../src/game/AudioManager.cpp \
../../../src/game/BaseAttacker.cpp \
../../../src/game/BlackHole.cpp \
../../../src/game/BulletManager.cpp \
../../../src/game/BulletManagerCollisionComponent.cpp \
../../../src/game/DefaultEntityRegistration.cpp \
../../../src/game/Defender.cpp \
../../../src/game/DefenderManager.cpp \
../../../src/game/ElectricityComponent.cpp \
../../../src/game/ElectricTower.cpp \
../../../src/game/FlameTower.cpp \
../../../src/game/FollowCamera.cpp \
../../../src/game/FPSContext.cpp \
../../../src/game/GameCamera.cpp \
../../../src/game/GameContext.cpp \
../../../src/game/GameData.cpp \
../../../src/game/GameEngine.cpp \
../../../src/game/GameManager.cpp \
../../../src/game/GameSettings.cpp \
../../../src/game/GroundCollisionComponent.cpp \
../../../src/game/GunTower.cpp \
../../../src/game/HQ.cpp \
../../../src/game/HQCollisionComponent.cpp \
../../../src/game/IAPContext.cpp \
../../../src/game/KamikazeAttacker.cpp \
../../../src/game/LaserComponent.cpp \
../../../src/game/LaserManager.cpp \
../../../src/game/LaserManagerCollisionComponent.cpp \
../../../src/game/LevelSelectButton.cpp \
../../../src/game/LevelSelectCamera.cpp \
../../../src/game/LevelSelectContext.cpp \
../../../src/game/MenuContext.cpp \
../../../src/game/NuclearTower.cpp \
../../../src/game/Pathway.cpp \
../../../src/game/PathwayGraphicComponent.cpp \
../../../src/game/PelletManager.cpp \
../../../src/game/Playfield.cpp \
../../../src/game/ProgressBar.cpp \
../../../src/game/ProgressIndicator.cpp \
../../../src/game/PulseManager.cpp \
../../../src/game/RangeIndicator.cpp \
../../../src/game/SFXManager.cpp \
../../../src/game/ShootingAttacker.cpp \
../../../src/game/SpawnPoint.cpp \
../../../src/game/StencilEffects.cpp \
../../../src/game/TankAttacker.cpp \
../../../src/game/Tower.cpp \
../../../src/game/UIContext.cpp \
../../../src/game/UIHandler.cpp \
../../../src/game/UIMarker.cpp \
../../../src/game/UIProgressBar.cpp \
../../../src/game/VKButton.cpp \
../../../src/game/VKeyboard.cpp \
../../../src/game/WaveManager.cpp \
../../../src/game/WheelMenu.cpp \
../../../src/game/WheelMenuButton.cpp \
../../../src/game/micropather/Micropather.cpp

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/../../../../shoot/build/android/Android.mk

