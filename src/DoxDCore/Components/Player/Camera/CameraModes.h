#pragma once

#include <CryRenderer/Tarray.h>
#include <CryEntitySystem/IEntityBasicTypes.h>

#include "ICameraMode.h"

struct SFocusCamSPParams;
struct IItem;

namespace DoxD
{
	struct CCameraPose
	{
	public:
		CCameraPose()
			: m_position(ZERO)
			, m_rotation(IDENTITY) {}
		CCameraPose(Vec3 positionOffset, Quat rotationOffset)
			: m_position(positionOffset)
			, m_rotation(rotationOffset) {}
		CCameraPose(const CCameraPose& copy)
			: m_position(copy.m_position)
			, m_rotation(copy.m_rotation) {}

		static CCameraPose Compose(const CCameraPose& lhv, const CCameraPose& rhv)
		{
			return CCameraPose(
				lhv.GetPosition() + rhv.GetPosition(),
				lhv.GetRotation() * rhv.GetRotation());
		}

		static CCameraPose Scale(const CCameraPose& lhv, float rhv)
		{
			if (rhv == 1.0f)
				return lhv;
			else
				return CCameraPose(
					lhv.GetPosition() * rhv,
					Quat::CreateSlerp(IDENTITY, lhv.GetRotation(), rhv));
		}

		static CCameraPose Lerp(const CCameraPose& from, const CCameraPose& to, float stride)
		{
			return CCameraPose(
				LERP(from.GetPosition(), to.GetPosition(), stride),
				Quat::CreateSlerp(from.GetRotation(), to.GetRotation(), stride));
		}

		Vec3 GetPosition() const { return m_position; }
		Quat GetRotation() const { return m_rotation; }

	private:
		Vec3 m_position;
		Quat m_rotation;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	class CDefaultCameraMode : public ICameraModeOld
	{
	public:
		CDefaultCameraMode(Cry::DefaultComponents::CCameraComponent* camera);
		virtual ~CDefaultCameraMode() {};

		virtual bool CanTransition() { return true; }
		virtual void GetCameraAnimationFactor(float& pos, float& rot)
		{
			pos = rot = 1.0f;
		}

		virtual void Update(const CPlayerComponentOld& player, float frameTime) override;

	private:
		float m_lastTpvDistance;
		float m_minAngle;
		float m_minAngleTarget;
		float m_minAngleRate;
		CTimeValue m_angleTransitionTimer;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	class CAnimationControlledCameraMode : public ICameraModeOld
	{
	public:
		CAnimationControlledCameraMode(Cry::DefaultComponents::CCameraComponent* camera);
		virtual ~CAnimationControlledCameraMode() {};
		virtual void Update(const CPlayerComponentOld& player, float frameTime) override;
		virtual bool CanTransition() { return true; }

		virtual void GetCameraAnimationFactor(float& pos, float& rot)
		{
			pos = 1.0f; rot = 1.0f;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	class CFocusCameraMode : public CAnimationControlledCameraMode
	{
	public:
		CFocusCameraMode(Cry::DefaultComponents::CCameraComponent* camera);
		virtual ~CFocusCameraMode();
		virtual void Update(const CPlayerComponentOld& player, float frameTime) override;
		virtual void  Activate(const CPlayerComponentOld& player);
		virtual void  Deactivate(const CPlayerComponentOld& player);

	private:
		typedef CAnimationControlledCameraMode ParentMode;

		bool	IsEntityInFrustrum(EntityId entityId, const CPlayerComponentOld& player) const;

		void  Init(const CPlayerComponentOld* subject, const EntityId killerEid);

	private:
		float	 m_fFocusDistance;
		float	 m_fFocusRange;

		//const SFocusCamSPParams& m_params;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	class CVehicleCameraMode : public ICameraModeOld
	{
	public:
		CVehicleCameraMode(Cry::DefaultComponents::CCameraComponent* camera);
		virtual void Update(const CPlayerComponentOld& player, float frameTime) override;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
}