
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := shoot
LOCAL_STATIC_LIBRARIES := zlib ck freetype cpufeatures

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../../include \
$(LOCAL_PATH)/../../include/OpenGL \
$(LOCAL_PATH)/../../include/rapidxml

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

ifeq ($(SHOOT_PROFILE), 1)
LOCAL_CFLAGS += -D_PROFILE_
endif

ifeq ($(SHOOT_RETAIL), 1)
LOCAL_CFLAGS += -D_RETAIL_
endif

#LOCAL_LDLIBS    := -llog -lGLESv2

LOCAL_SRC_FILES := \
../../src/AABBox2D.cpp \
../../src/AABBox3D.cpp \
../../src/Alloc.cpp \
../../src/AnimationVisitor.cpp \
../../src/AudioDriver.cpp \
../../src/Basis3.cpp \
../../src/BillBoardEntity.cpp \
../../src/Button.cpp \
../../src/Camera.cpp \
../../src/CameraPath.cpp \
../../src/CameraPathElement.cpp \
../../src/CameraSpectator.cpp \
../../src/CameraVisitor.cpp \
../../src/Clock.cpp \
../../src/CollisionComponent.cpp \
../../src/CollisionManager.cpp \
../../src/CollisionShape.cpp \
../../src/CollisionUtils.cpp \
../../src/Color.cpp \
../../src/ColorVisitor.cpp \
../../src/CombineVisitor.cpp \
../../src/Component.cpp \
../../src/Contact.cpp \
../../src/Context.cpp \
../../src/ContextStack.cpp \
../../src/CubeMapResource.cpp \
../../src/Debug.cpp \
../../src/DelayVisitor.cpp \
../../src/Effect.cpp \
../../src/EffectEntity.cpp \
../../src/Engine.cpp \
../../src/Entity.cpp \
../../src/Entity2D.cpp \
../../src/Entity3D.cpp \
../../src/EntityRenderer.cpp \
../../src/EntityRenderer3D.cpp \
../../src/EntityRendererFull.cpp \
../../src/EntityVisitor.cpp \
../../src/EnvironmentCollisionComponent.cpp \
../../src/Event.cpp \
../../src/EventManager.cpp \
../../src/ExplosionVisitor.cpp \
../../src/ExtendedPropeties.cpp \
../../src/FadeRenderer.cpp \
../../src/FadeVisitor.cpp \
../../src/FakeCubeMapTexture.cpp \
../../src/File.cpp \
../../src/FileAPK.cpp \
../../src/FileFS.cpp \
../../src/FileLibC.cpp \
../../src/FileSystem.cpp \
../../src/FollowCameraPathVisitor.cpp \
../../src/FollowPathVisitor.cpp \
../../src/FontManager.cpp \
../../src/Frustum.cpp \
../../src/GeometryProvider.cpp \
../../src/GraphicComponent.cpp \
../../src/GraphicExtensionHandler.cpp \
../../src/GraphicObject.cpp \
../../src/GraphicsDriver.cpp \
../../src/Handle.cpp \
../../src/HighResTimer.cpp \
../../src/Image.cpp \
../../src/InputManager.cpp \
../../src/InputManagerTouch.cpp \
../../src/InputRecorder.cpp \
../../src/LayoutComponent.cpp \
../../src/Light.cpp \
../../src/LightVisitor.cpp \
../../src/Line.cpp \
../../src/LoadContextVisitor.cpp \
../../src/Log.cpp \
../../src/Material.cpp \
../../src/MaterialParameter.cpp \
../../src/MaterialProvider.cpp \
../../src/MaterialVisitor.cpp \
../../src/MathUtils.cpp \
../../src/Matrix44.cpp \
../../src/MemoryManager.cpp \
../../src/MemoryTexture.cpp \
../../src/MenuRootContext.cpp \
../../src/MeshEntity.cpp \
../../src/MeshResource.cpp \
../../src/Music.cpp \
../../src/Mutex.cpp \
../../src/MutexPT.cpp \
../../src/NetworkManager.cpp \
../../src/Object.cpp \
../../src/ObjectFactory.cpp \
../../src/ObjectManager.cpp \
../../src/OpenGL/OpenGLDriverES2.cpp \
../../src/OpenGL/OpenGLExtensionHandler.cpp \
../../src/OpenGL/OpenGLExtensionHandlerES2.cpp \
../../src/OpenGL/OpenGLMemoryTexture.cpp \
../../src/OpenGL/OpenGLRenderTargetES2.cpp \
../../src/OpenGL/OpenGLShader.cpp \
../../src/OpenGL/OpenGLTexture.cpp \
../../src/OpenGL/OpenGLVertexBuffer.cpp \
../../src/OpenGL/OpenGLVertexBufferNoVAO.cpp \
../../src/OpenGL/SkyBoxVertexBufferNoCubemap.cpp \
../../src/OpenGL/SkyBoxVertexBufferNoCubemap_NoVAO.cpp \
../../src/ParticleGenerator.cpp \
../../src/ParticleVisitor.cpp \
../../src/Path.cpp \
../../src/PathElement.cpp \
../../src/Plane.cpp \
../../src/Point.cpp \
../../src/ProfileManager.cpp \
../../src/Projection.cpp \
../../src/Property.cpp \
../../src/PropertyStream.cpp \
../../src/QuadTree.cpp \
../../src/Quaternion.cpp \
../../src/Random.cpp \
../../src/ReceiveTriggerVisitor.cpp \
../../src/Reference.cpp \
../../src/RemoveEntityVisitor.cpp \
../../src/RenderableEntity.cpp \
../../src/RenderTarget.cpp \
../../src/Resource.cpp \
../../src/ResourceManager.cpp \
../../src/RigidBodyComponent.cpp \
../../src/Rotation2DVisitor.cpp \
../../src/Rotation3DVisitor.cpp \
../../src/Scale2DVisitor.cpp \
../../src/Scale3DVisitor.cpp \
../../src/Semaphore.cpp \
../../src/SemaphorePT.cpp \
../../src/SendTriggerVisitor.cpp \
../../src/SequenceVisitor.cpp \
../../src/Shader.cpp \
../../src/ShaderParameter.cpp \
../../src/Shake3DVisitor.cpp \
../../src/SkyBoxEntity.cpp \
../../src/Sound.cpp \
../../src/SoundBank.cpp \
../../src/Sprite.cpp \
../../src/StatsManager.cpp \
../../src/Text.cpp \
../../src/Text3D.cpp \
../../src/Texture.cpp \
../../src/TextureAnimationVisitor.cpp \
../../src/TextVisitor.cpp \
../../src/Thread.cpp \
../../src/ThreadPT.cpp \
../../src/TimeManager.cpp \
../../src/Timer.cpp \
../../src/TrailEntity.cpp \
../../src/Translation2DVisitor.cpp \
../../src/Translation3DVisitor.cpp \
../../src/Triangle.cpp \
../../src/UISlider.cpp \
../../src/Utils.cpp \
../../src/Vector2.cpp \
../../src/Vector3.cpp \
../../src/Vector4.cpp \
../../src/VertexBuffer.cpp \
../../src/Visitor.cpp \
../../src/Gamepad.cpp

include $(BUILD_STATIC_LIBRARY)

SHOOT_BUILD_PATH := $(LOCAL_PATH)
include $(SHOOT_BUILD_PATH)/zlib/Android.mk
include $(SHOOT_BUILD_PATH)/../../utils/cricket/build/android/Android.mk
include $(SHOOT_BUILD_PATH)/../../utils/freetype/android/Android.mk

$(call import-module, android/cpufeatures)


