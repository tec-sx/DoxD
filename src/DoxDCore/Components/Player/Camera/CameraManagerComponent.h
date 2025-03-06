/**
\file	Source\Actor\PlayerCamera\PlayerCamera.h

This is a camera management class. It creates several cameras, one of each type, and then provides features
for switching between the cameras. While the name implies the cameras are just for the player, in general they will
be able to follow any entity in the game.

The host entity will be used as a default for the entity which the camera operates from.
*/
#pragma once

#include <IGameObject.h>
#include "ICameraComponent.h"
#include <DefaultComponents/Input/InputComponent.h>


namespace DoxD
{
	/** A camera manager component. */
	class CCameraManagerComponent : public IEntityComponent
	{
	protected:
		// IEntityComponent
		void Initialize() override;
		void ProcessEvent(const SEntityEvent& event) override;
		Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Update; }
		// ~IEntityComponent

		void RegisterActionMaps();

	public:
		CCameraManagerComponent();
		virtual ~CCameraManagerComponent() {}

		static void ReflectType(Schematyc::CTypeDesc<CCameraManagerComponent>& desc)
		{
			desc.SetGUID(CCameraManagerComponent::IID());
			desc.SetEditorCategory("Hidden");
			desc.SetLabel("Camera Manager");
			desc.SetDescription("No description.");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags({ 
				IEntityComponent::EFlags::Singleton, 
				IEntityComponent::EFlags::Socket, 
				IEntityComponent::EFlags::Attach, 
				IEntityComponent::EFlags::ClientOnly });
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{681AA5B4-9746-4F2C-B19E-AE1B47688AD2}"_cry_guid;
			return id;
		}

		void Update();
		void AttachToEntity(EntityId entityID);

		ECameraMode GetCameraMode() { return m_cameraMode; }
		void SetCameraMode(ECameraMode mode, const char* reason);
		void SetLastCameraMode();

		ICameraComponent* GetCamera() const;
		float GetZoomDelta() { return m_lastZoomDelta; };
		Vec3 GetViewOffset();

		/**	Camera debug actions. **/
		bool OnActionCameraShiftUp(int activationMode, float value);
		bool OnActionCameraShiftDown(int activationMode, float value);
		bool OnActionCameraShiftLeft(int activationMode, float value);
		bool OnActionCameraShiftRight(int activationMode, float value);
		bool OnActionCameraShiftForward(int activationMode, float value);
		bool OnActionCameraShiftBackward(int activationMode, float value);


	private:
		CPlayerComponent* m_pPlayer{ nullptr };
		Cry::DefaultComponents::CInputComponent* m_pInputComponent{ nullptr };
		ICameraComponent* m_cameraModes[eCameraMode_Last];
		
		ECameraMode m_cameraMode{ eCameraMode_Invalid };
		ECameraMode m_lastCameraMode{ eCameraMode_Invalid };

		const float m_adjustmentAmount{ 0.025f };
		Vec3 m_interactiveViewOffset{ ZERO };
		float m_zoomDelta{ 0.0f };
		float m_lastZoomDelta{ 0.0f };
	};
}