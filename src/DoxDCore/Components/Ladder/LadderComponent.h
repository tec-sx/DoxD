#pragma once

#include <DefaultComponents/Geometry/StaticMeshComponent.h>

const char* CUBE_PATH = "%ENGINE%/EngineAssets/Objects/primitive_cube.cgf";
const char* CYLINDER_PATH = "%ENGINE%/EngineAssets/Objects/primitive_cylinder.cgf";
const char* MATERIAL_PATH = "%ENGINE%/EngineAssets/Objects/editorprimitive";

enum LadderGeometrySlot
{
	POLE_L_GEOMETRY_SLOT = 0,
	POLE_R_GEOMETRY_SLOT = 1,
	BAR_GEOMETRY_SLOT = 2
};

// Ladder Component
class CLadderComponent : public IEntityComponent
{
public:
	CLadderComponent() = default;
	~CLadderComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CLadderComponent>& desc)
	{
		desc.SetGUID("{3A998AF2-430F-4A8B-BCB3-8132DAC4D30B}"_cry_guid);
		desc.SetEditorCategory("LevelDesign");
		desc.SetLabel("Ladder");
		desc.SetDescription("Procedural Ladder Component");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

		// Poles
		desc.AddMember(&CLadderComponent::m_poleScale, 'plsz', "PoleSize", "Pole Size", "The size of each of the ladder poles.", Vec3(0.05f, 0.07f, 0.5f));
		desc.AddMember(&CLadderComponent::m_poleDistance, 'plds', "PoleDistance", "Pole Distance", "The distance of the ladder poles from the center.", 0.3f);
		desc.AddMember(&CLadderComponent::m_poleGeometryPath, 'pgeo', "PoleGeometryPath", "Pole Geometry path", "Geometry path of this item", CUBE_PATH);

		// Bars
		desc.AddMember(&CLadderComponent::m_barSize, 'brsz', "BarSize", "Bar size", "The size of the cross-bars.", 0.04f);
		desc.AddMember(&CLadderComponent::m_barDistance, 'brdp', "BarDepth", "Bar Depth", "The disance between the bars.", 0.5f);
		desc.AddMember(&CLadderComponent::m_barGeometryPath, 'bgeo', "BarGeometryPath", "Bar Geometry path", "Geometry path of this item", CYLINDER_PATH);

		// Common
		desc.AddMember(&CLadderComponent::m_materialPath, 'matl', "MaterialPath", "Material path", "Material path of this item", MATERIAL_PATH);
	}

	virtual void Initialize();
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	void Reset();

private:
	// Pole properties
	Schematyc::GeomFileName m_poleGeometryPath = CUBE_PATH;
	Vec3 m_poleScale = Vec3(0.05f, 0.07f, 0.5f);
	float m_poleDistance = 0.3f;
	_smart_ptr<IStatObj> m_poleGeom;

	// Bar properties
	Schematyc::GeomFileName m_barGeometryPath = CYLINDER_PATH;
	float m_barSize = 0.04f;
	float m_barDistance = 0.5f;
	_smart_ptr<IStatObj> m_barGeom;

	// Common properties
	Schematyc::MaterialFileName m_materialPath = MATERIAL_PATH;
	_smart_ptr<IMaterial> m_material;
};

