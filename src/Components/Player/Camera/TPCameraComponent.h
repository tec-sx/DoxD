#pragma once
#include "ICameraComponent.h"

namespace DoxD
{
	class CCameraManagerComponent;
	 
	class CTPCameraComponent : public ICameraComponent
	{
	protected:
		// IEntityComponent
		void Initialize() override;
		void ProcessEvent(const SEntityEvent& event) override;
		Cry::Entity::EventFlags GetEventMask() const override { return m_EventFlags; }
		void OnShutDown() override;
		// ~IEntityComponent

	public:
		CTPCameraComponent() {}
		virtual ~CTPCameraComponent() {}

		static void ReflectType(Schematyc::CTypeDesc<CTPCameraComponent>& desc)
		{
			desc.SetGUID(CTPCameraComponent::IID());
			desc.SetEditorCategory("Cameras");
			desc.SetLabel("Third Person Camera");
			desc.SetDescription("An action RPG style of camera that orbits around a target.");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{89124BA1-6830-4552-B983-2B9FCFFF107B}"_cry_guid;
			return id;
		}

		virtual void AttachToEntity(EntityId entityID);

		// ICameraComponent
		void OnActivate() override;
		void OnDeactivate() override;
		// ~ICameraComponent

		virtual void ResetCamera();
		Vec3 GetTargetAimPosition(IEntity* const pEntity);

		bool DetectCollision(const Vec3& Goal, Vec3& CameraPosition);
	private:
		void Update();
		void UpdateZoom();
		void UpdateCamera();

		CCameraManagerComponent* m_pCameraManager{ nullptr };
		EntityId m_targetEntityID{ INVALID_ENTITYID };
		
		Vec3 m_vecLastPosition{ ZERO };
		Vec3 m_vecLastTargetPosition{ ZERO };
		Vec3 m_vecLastTargetAimPosition{ ZERO };

		Quat m_quatTargetRotation{ IDENTITY };
		Quat m_quatLastTargetRotation{ IDENTITY };
		Quat m_quatLastViewRotation{ IDENTITY };

		/**
		Set to true when you want to avoid using interpolation for the update next frame. There are times when we don't
		have a reasonable value for the last camera position / rotation and this allows us to get around those awkward
		moments.
		*/
		bool m_skipInterpolation{ true };

		float m_zoomGoal;
		float m_lastZoomGoal;
		float m_zoomLevel;

		float m_viewPitch;
		float m_viewYaw;

		Cry::Entity::EventFlags m_EventFlags;
	};
}

