#pragma once

#include <IActionMapManager.h>
#include <DefaultComponents/Input/InputComponent.h>

namespace DoxD
{
	class CPlayerComponentOld;
	class CCameraManagerComponent;

	enum class EInputFlagType
	{
		Hold = 0,
		Toggle
	};

	typedef uint8 TInputFlags;

	enum class EInputFlag
		: TInputFlags
	{
		None = 0,
		Left = 1 << 0,
		Right = 1 << 1,
		Forward = 1 << 2,
		Backward = 1 << 3
	};


	class CPlayerInputComponent : public IEntityComponent
	{
	protected:
		// IEntityComponent
		void Initialize() override;
		void ProcessEvent(const SEntityEvent& event) override;
		Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Update; }
		// ~IEntityComponent

		void Update();

	public:
		CPlayerInputComponent() = default;
		virtual ~CPlayerInputComponent() = default;

		static void ReflectType(Schematyc::CTypeDesc<CPlayerInputComponent>& desc)
		{
			desc.SetGUID(CPlayerInputComponent::IID());
			desc.SetEditorCategory("Hidden");
			desc.SetLabel("Player Input");
			desc.SetDescription("No description.");
			desc.SetIcon("icons:ObjectTypes/light.ico");
			desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
		}

		static CryGUID& IID()
		{
			static CryGUID id = "{83B1A049-99A7-4860-86A5-3FF83834636F}"_cry_guid;
			return id;
		}

		bool IsMovementRequested() const {
			return m_inputFlags != 0;
		}

		Vec3 GetMovementDirection(const Quat& baseRotation);
		Ang3 GetRotationDelta() { return Ang3(m_mousePitchDelta, 0.0f, m_mouseYawDelta); }
		float GetMousePitchDelta() { return m_mousePitchDelta; }
		float GetMouseYawDelta() { return m_mouseYawDelta; };
		TInputFlags GetMovementDirectionFlags() const { return m_inputFlags; };

		/** Listen for important keys and be notified when they are input e.g. ESC, interact, spellcast. */
		struct IInputEventListener
		{
			virtual void OnInputEscape(int activationMode) = 0;
			virtual void OnInputInteraction(int activationMode) = 0;
		};


		void AddEventListener(IInputEventListener* pListener)
		{
			stl::push_back_unique(m_listeners, pListener);
		}

		void RemoveEventListener(IInputEventListener* pListener)
		{
			m_listeners.remove(pListener);
		}


	protected:
		void OnActionRotateYaw(int activationMode, float value);
		void OnActionRotatePitch(int activationMode, float value);
		
		void OnActionEscape(int activationMode, float value);
		void OnActionInteraction(int activationMode, float value);

		void OnActionJump(int activationMode, float value);
		void OnActionCrouchToggle(int activationMode, float value);
		void OnActionCrawlToggle(int activationMode, float value);
		void OnActionSitToggle(int activationMode, float value);
		void OnActionSprintToggle(int activationMode, float value);
		void OnActionWalkJog(int activationMode, float value);

		void OnActionItemUse(int activationMode, float value);
		void OnActionItemPickup(int activationMode, float value);
		void OnActionItemDrop(int activationMode, float value);
		void OnActionItemToss(int activationMode, float value);

		void OnNumpad(int activationMode, int buttonId);

		void OnActionInspectStart(int activationMode, float value);
		void OnActionInspect(int activationMode, float value);
		void OnActionInspectEnd(int activationMode, float value);

		CPlayerComponentOld* m_pPlayer{ nullptr };

	private:
		void RegisterActionMaps();
		void HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type = EInputFlagType::Hold);

		Cry::DefaultComponents::CInputComponent* m_pInputComponent{ nullptr };

		TInputFlags m_inputFlags{ (TInputFlags)EInputFlag::None };


		int m_mouseInvertPitch{ 0 };
		float m_mousePitchYawSensitivity{ 1.0f };

		float m_mousePitchDelta{ 0.0f };
		float m_mouseYawDelta{ 0.0f };

		float m_pitchFilter{ 0.0001f };
		float m_yawFilter{ 0.0001f };

		std::list<IInputEventListener*> m_listeners;
	};
}