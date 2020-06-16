#ifndef THAWK_ENGINE_COMPONENTS_H
#define THAWK_ENGINE_COMPONENTS_H

#include "../engine.h"

namespace Tomahawk
{
	namespace Engine
	{
		namespace Components
		{
			class Model;

			class SkinModel;

			class THAWK_OUT RigidBody : public Component
			{
			private:
				Compute::UnmanagedShape* Hull;
				Compute::RigidBody* Instance;

			public:
				bool Synchronize;
				bool Kinematic;

			public:
				RigidBody(Entity* Ref);
				virtual ~RigidBody() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual void OnAsleep() override;
				virtual Component* OnClone(Entity* New) override;
				void Initialize(btCollisionShape* Shape, float Mass, float Anticipation);
				void Initialize(ContentManager* Content, const std::string& Path, float Mass, float Anticipation);
				void Clear();
				void SetTransform(const Compute::Matrix4x4& World);
				void SetTransform(bool Kinematic);
				void SetMass(float Mass);
				Compute::RigidBody* GetBody() const;

			public:
				THAWK_COMPONENT(RigidBody);
			};

			class THAWK_OUT SoftBody : public Component
			{
			private:
				Compute::UnmanagedShape* Hull;
				Compute::SoftBody* Instance;
				std::vector<Compute::Vertex> Vertices;
				std::vector<int> Indices;

			public:
				Appearance Surface;
				bool Synchronize;
				bool Kinematic;
				bool Visibility;

			public:
				SoftBody(Entity* Ref);
				virtual ~SoftBody() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual void OnAsleep() override;
				virtual Component* OnClone(Entity* New) override;
				void InitializeShape(Compute::UnmanagedShape* Shape, float Anticipation);
				void InitializeShape(ContentManager* Content, const std::string& Path, float Anticipation);
				void InitializeEllipsoid(const Compute::SoftBody::Desc::CV::SEllipsoid& Shape, float Anticipation);
				void InitializePatch(const Compute::SoftBody::Desc::CV::SPatch& Shape, float Anticipation);
				void InitializeRope(const Compute::SoftBody::Desc::CV::SRope& Shape, float Anticipation);
				void Fill(Graphics::GraphicsDevice* Device, Graphics::ElementBuffer* IndexBuffer, Graphics::ElementBuffer* VertexBuffer);
				void Clear();
				void SetTransform(const Compute::Matrix4x4& World);
				void SetTransform(bool Kinematic);
				Graphics::Material& GetMaterial();
				Compute::SoftBody* GetBody() const;
				std::vector<Compute::Vertex>& GetVertices();
				std::vector<int>& GetIndices();

			public:
				THAWK_COMPONENT(SoftBody);
			};

			class THAWK_OUT Acceleration : public Component
			{
			private:
				Compute::RigidBody* RigidBody;

			public:
				Compute::Vector3 AmplitudeVelocity;
				Compute::Vector3 AmplitudeTorque;
				Compute::Vector3 ConstantVelocity;
				Compute::Vector3 ConstantTorque;
				Compute::Vector3 ConstantCenter;
				bool Velocity;

			public:
				Acceleration(Entity* Ref);
				virtual ~Acceleration() = default;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnAwake(Component* New) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				Compute::RigidBody* GetBody() const;

			public:
				THAWK_COMPONENT(Acceleration);
			};

			class THAWK_OUT SliderConstraint : public Component
			{
			private:
				Compute::SliderConstraint* Instance;
				Entity* Connection;

			public:
				SliderConstraint(Entity* Ref);
				virtual ~SliderConstraint() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual Component* OnClone(Entity* New) override;
				void Initialize(bool IsGhosted, bool IsLinear);
				void Clear();
				Compute::SliderConstraint* GetConstraint() const;
				Entity* GetConnection() const;

			public:
				THAWK_COMPONENT(SliderConstraint);
			};

			class THAWK_OUT AudioSource : public Component
			{
			private:
				Compute::Vector3 LastPosition;
				Audio::AudioSource* Source;
				Audio::AudioSync Sync;

			public:
				AudioSource(Entity* Ref);
				virtual ~AudioSource() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				void ApplyPlayingPosition();
				Audio::AudioSource* GetSource() const;
				Audio::AudioSync& GetSync();

			public:
				THAWK_COMPONENT(AudioSource);
			};

			class THAWK_OUT AudioListener : public Component
			{
			private:
				Compute::Vector3 LastPosition;

			public:
				float Gain;

			public:
				AudioListener(Entity* Ref);
				virtual ~AudioListener() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual void OnAsleep() override;
				virtual Component* OnClone(Entity* New) override;

			public:
				THAWK_COMPONENT(AudioListener);
			};

			class THAWK_OUT SkinAnimator : public Component
			{
			private:
				SkinModel* Instance = nullptr;

			public:
				std::vector<Compute::SkinAnimatorClip> Clips;
				std::vector<Compute::AnimatorKey> Current;
				std::vector<Compute::AnimatorKey> Bind;
				std::vector<Compute::AnimatorKey> Default;
				AnimatorState State;

			public:
				SkinAnimator(Entity* Ref);
				virtual ~SkinAnimator() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnAwake(Component* New) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				void Play(int64_t Clip = -1, int64_t Frame = -1);
				void Pause();
				void Stop();
				std::vector<Compute::AnimatorKey>* GetFrame(int64_t Clip, int64_t Frame);
				std::vector<std::vector<Compute::AnimatorKey>>* GetClip(int64_t Clip);
				SkinModel* GetAnimatedObject() const;

			private:
				void BlendAnimation(int64_t Clip, int64_t Frame);
				void SavePose();
				bool IsPosed(int64_t Clip, int64_t Frame);

			public:
				THAWK_COMPONENT(SkinAnimator);
			};

			class THAWK_OUT KeyAnimator : public Component
			{
			public:
				std::vector<Compute::KeyAnimatorClip> Clips;
				Compute::AnimatorKey Current, Bind;
				AnimatorState State;

			public:
				KeyAnimator(Entity* Ref);
				virtual ~KeyAnimator() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				void Play(int64_t Clip = -1, int64_t Frame = -1);
				void Pause();
				void Stop();
				Compute::AnimatorKey* GetFrame(int64_t Clip, int64_t Frame);
				std::vector<Compute::AnimatorKey>* GetClip(int64_t Clip);

			private:
				void BlendAnimation(int64_t Clip, int64_t Frame);
				void SavePose();
				bool IsPosed(int64_t Clip, int64_t Frame);

			public:
				THAWK_COMPONENT(KeyAnimator);
			};

			class THAWK_OUT ElementSystem : public Component
			{
			private:
				Graphics::InstanceBuffer* Instance;

			public:
				Appearance Surface;
				float Visibility;
				float Volume;
				bool Connected;
				bool QuadBased;

			public:
				ElementSystem(Entity* Ref);
				virtual ~ElementSystem();
				virtual void OnAwake(Component* New) override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual void OnEvent(Event* Value) override;
				virtual Component* OnClone(Entity* New) override;
				Graphics::Material& GetMaterial();
				Graphics::InstanceBuffer* GetBuffer() const;

			public:
				THAWK_COMPONENT(ElementSystem);
			};

			class THAWK_OUT ElementAnimator : public Component
			{
			private:
				ElementSystem* System;

			public:
				Compute::Vector4 Diffuse;
				Compute::Vector3 Position;
				Compute::Vector3 Velocity;
				SpawnerProperties Spawner;
				float RotationSpeed;
				float ScaleSpeed;
				float Noisiness;
				bool Simulate;

			public:
				ElementAnimator(Entity* Ref);
				virtual ~ElementAnimator() = default;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnAwake(Component* New) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				ElementSystem* GetSystem() const;

			protected:
				void AccurateSynchronization(float DeltaTime);
				void FastSynchronization(float DeltaTime);

			public:
				THAWK_COMPONENT(ElementAnimator);
			};

			class THAWK_OUT FreeLook : public Component
			{
			private:
				Graphics::Activity* Activity;
				Compute::Vector2 Position;

			public:
				Graphics::KeyMap Rotate;
				float Sensitivity;

			public:
				FreeLook(Entity* Ref);
				virtual ~FreeLook() = default;
				virtual void OnAwake(Component* New) override;
				virtual void OnUpdate(Rest::Timer* Time) override;
				Graphics::Activity* GetActivity() const;

			public:
				THAWK_COMPONENT(FreeLook);
			};

			class THAWK_OUT Fly : public Component
			{
			private:
				Graphics::Activity* Activity;

			public:
				Graphics::KeyMap Forward;
				Graphics::KeyMap Backward;
				Graphics::KeyMap Right;
				Graphics::KeyMap Left;
				Graphics::KeyMap Up;
				Graphics::KeyMap Down;
				Graphics::KeyMap Fast;
				Graphics::KeyMap Slow;
				Compute::Vector3 Axis;
				float SpeedNormal;
				float SpeedUp;
				float SpeedDown;

			public:
				Fly(Entity* Ref);
				virtual ~Fly() = default;
				virtual void OnAwake(Component* New) override;
				virtual void OnUpdate(Rest::Timer* Time) override;
				Graphics::Activity* GetActivity() const;

			public:
				THAWK_COMPONENT(Fly);
			};

			class THAWK_OUT Model : public Component
			{
			private:
				Graphics::Model* Instance = nullptr;

			public:
				std::unordered_map<Graphics::MeshBuffer*, Appearance> Surfaces;
				bool Visibility;

			public:
				Model(Entity* Ref);
				virtual ~Model() = default;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual void OnEvent(Event* Value) override;
				virtual Component* OnClone(Entity* New) override;
				void SetDrawable(Graphics::Model* Drawable);
				Graphics::Material& GetMaterial(Graphics::MeshBuffer* Mesh);
				Graphics::Material& GetMaterial(Appearance* Surface);
				Appearance* GetSurface(Graphics::MeshBuffer* Mesh);
				Compute::Matrix4x4 GetBoundingBox();
				Graphics::Model* GetDrawable() const;

			public:
				THAWK_COMPONENT(Model);
			};

			class THAWK_OUT SkinModel : public Component
			{
			private:
				Graphics::SkinModel* Instance = nullptr;

			public:
				std::unordered_map<Graphics::SkinMeshBuffer*, Appearance> Surfaces;
				Graphics::PoseBuffer Skeleton;
				bool Visibility;

			public:
				SkinModel(Entity* Ref);
				virtual ~SkinModel() = default;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual void OnEvent(Event* Value) override;
				virtual Component* OnClone(Entity* New) override;
				void SetDrawable(Graphics::SkinModel* Drawable);
				Graphics::Material& GetMaterial(Graphics::SkinMeshBuffer* Mesh);
				Graphics::Material& GetMaterial(Appearance* Surface);
				Appearance* GetSurface(Graphics::SkinMeshBuffer* Mesh);
				Compute::Matrix4x4 GetBoundingBox();
				Graphics::SkinModel* GetDrawable() const;

			public:
				THAWK_COMPONENT(SkinModel);
			};

			class THAWK_OUT PointLight : public Component
			{
			private:
				Graphics::Texture2D* ShadowCache;

			public:
				Compute::Matrix4x4 View;
				Compute::Matrix4x4 Projection;
				Compute::Vector3 Diffuse;
				float Visibility = 0.0f;
				float Emission = 1.0f;
				float Range = 5.0f;
				float ShadowSoftness = 0.0f;
				float ShadowDistance = 10.0f;
				float ShadowBias = 0.0f;
				int ShadowIterations = 2;
				bool Shadowed = false;

			public:
				PointLight(Entity* Ref);
				virtual ~PointLight() = default;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				void SetShadowCache(Graphics::Texture2D* NewCache);
				Graphics::Texture2D* GetShadowCache() const;

			public:
				THAWK_COMPONENT(PointLight);
			};

			class THAWK_OUT SpotLight : public Component
			{
			private:
				Graphics::Texture2D* ShadowCache;

			public:
				Graphics::Texture2D* ProjectMap;
				Compute::Matrix4x4 Projection;
				Compute::Matrix4x4 View;
				Compute::Vector3 Diffuse;
				float ShadowSoftness;
				float ShadowDistance;
				float ShadowBias;
				float FieldOfView;
				float Emission;
				float Range;
				float Visibility;
				int ShadowIterations;
				bool Shadowed;

			public:
				SpotLight(Entity* Ref);
				virtual ~SpotLight() = default;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				void SetShadowCache(Graphics::Texture2D* NewCache);
				Graphics::Texture2D* GetShadowCache() const;

			public:
				THAWK_COMPONENT(SpotLight);
			};

			class THAWK_OUT LineLight : public Component
			{
			private:
				Graphics::Texture2D* ShadowCache;

			public:
				Compute::Matrix4x4 Projection;
				Compute::Matrix4x4 View;
				Compute::Vector3 Diffuse;
				float ShadowDistance;
				float ShadowSoftness;
				float ShadowBias;
				float ShadowFarBias;
				float ShadowLength;
				float ShadowHeight;
				float Emission;
				int ShadowIterations;
				bool Shadowed;

			public:
				LineLight(Entity* Ref);
				virtual ~LineLight() = default;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				void SetShadowCache(Graphics::Texture2D* NewCache);
				Graphics::Texture2D* GetShadowCache() const;

			public:
				THAWK_COMPONENT(LineLight);
			};

			class THAWK_OUT ProbeLight : public Component
			{
			private:
				Graphics::Texture2D* DiffuseMapX[2];
				Graphics::Texture2D* DiffuseMapY[2];
				Graphics::Texture2D* DiffuseMapZ[2];
				Graphics::TextureCube* ProbeCache;

			public:
				Compute::Matrix4x4 View[6];
				Compute::Matrix4x4 Projection;
				Compute::Vector3 Diffuse;
				Compute::Vector3 ViewOffset;
				Rest::TickTimer Rebuild;
				float CaptureRange;
				float Emission;
				float Range;
				float Visibility;
				bool ParallaxCorrected;
				bool RenderLocked;

			public:
				ProbeLight(Entity* Ref);
				virtual ~ProbeLight() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual Component* OnClone(Entity* New) override;
				void SetProbeCache(Graphics::TextureCube* NewCache);
				bool SetDiffuseMap(Graphics::Texture2D* MapX[2], Graphics::Texture2D* MapY[2], Graphics::Texture2D* MapZ[2]);
				bool IsImageBased() const;
				Graphics::TextureCube* GetProbeCache() const;

			public:
				THAWK_COMPONENT(ProbeLight);
			};

			class THAWK_OUT Camera : public Component
			{
			public:
				enum ProjectionMode
				{
					ProjectionMode_Perspective,
					ProjectionMode_Orthographic
				} Mode;

			protected:
				RenderSystem* Renderer = nullptr;
				Compute::Matrix4x4 Projection;
				Viewer FieldView;

			public:
				float NearPlane, FarPlane;
				float Width, Height;
				float FieldOfView;

			public:
				Camera(Entity* Ref);
				virtual ~Camera() override;
				virtual void OnLoad(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnSave(ContentManager* Content, Rest::Document* Node) override;
				virtual void OnAwake(Component* New) override;
				virtual void OnSynchronize(Rest::Timer* Time) override;
				virtual void OnAsleep() override;
				virtual Component* OnClone(Entity* New) override;
				void FillViewer(Viewer* View);
				void ResizeBuffers();
				Viewer GetViewer();
				RenderSystem* GetRenderer();
				Compute::Matrix4x4 GetProjection();
				Compute::Matrix4x4 GetViewProjection();
				Compute::Matrix4x4 GetView();
				Compute::Vector3 GetViewPosition();

			public:
				THAWK_COMPONENT(Camera);
			};
		}
	}
}
#endif