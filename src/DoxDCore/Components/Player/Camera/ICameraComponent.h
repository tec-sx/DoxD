#pragma once
#include <CryMath/Cry_Math.h>
#include <CryRenderer/Tarray.h>
#include <DefaultComponents/Audio/ListenerComponent.h>

namespace DoxD
{
	class CPlayerComponent;
	struct SViewParams;

	enum ECameraMode
	{
		eCameraMode_Invalid = -1,
		eCameraMode_Default = 0,
		eCameraMode_Examine,
		eCameraMode_Vehicle,
		eCameraMode_AnimationControlled,
		eCameraMode_Last
	};

	const Vec3 AverageEyePosition{ 0.0f, 0.0f, 1.82f };

	struct ICameraComponent : IEntityComponent
	{
	public:
		ICameraComponent() {}
		virtual ~ICameraComponent() {}

		static void ReflectType(Schematyc::CTypeDesc<ICameraComponent>& desc)
		{
			desc.SetGUID(ICameraComponent::IID());
			desc.SetEditorCategory("Hidden");
			desc.SetLabel("Camera Component Interface");
			desc.SetDescription("No description.");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{5676B08A-CDDA-4145-BDB0-0AABA4A8EE8F}"_cry_guid;
			return id;
		}

		struct AnimationSettings
		{
			AnimationSettings() : positionFactor(0.f), rotationFactor(0.f), applyResult(false), stableBlendOff(false) {}

			float positionFactor;
			float rotationFactor;
			bool applyResult;
			bool stableBlendOff;
		};

		virtual void AttachToEntity(EntityId entityID) = 0;
		
		bool IsDebugEnabled(void) const { return (m_isDebugEnabled); };
		void SetDebugEnabled(const bool enabled) { m_isDebugEnabled = enabled; };
		
		virtual bool CanTransition() { return false; };
		const bool IsBlendingOff() const { return m_bBlendingOff; }
		
		virtual void SetCameraAnimationFactor(const AnimationSettings& animationSettings) {};
		virtual void GetCameraAnimationFactor(float& position, float& rotation);

		virtual void OnActivate() = 0;
		virtual void OnDeactivate() = 0;
		
		CCamera& GetCamera() { return m_camera; }
		const CCamera& GetCamera() const { return m_camera; }

		Vec3 const GetPosition() const { return m_cameraMatrix.GetTranslation(); }
		Quat const GetRotation() const { return Quat(m_cameraMatrix); }
		const Vec3 GetAimTarget(const IEntity* pRayCastingEntity) const;
		void SetFieldOfView(float fov) { m_fieldOfView = fov; }

		virtual void UpdateView();

	protected:
		CCamera m_camera;

		Matrix34 m_cameraMatrix{ ZERO, IDENTITY };
		Schematyc::Range<0, 32768> m_nearPlane = 0.25f;
		Schematyc::Range<20, 120> m_fieldOfView = 75.0f;
		
		bool m_isDebugEnabled{ false };
		bool m_bBlendingOff{ false };
	};
}

