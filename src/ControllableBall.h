#pragma once
#include "WO.h"

namespace physx { class PxPhysics; class PxActor; class PxRigidDynamic; class PxMaterial; class PxShape; }

namespace Aftr {
	class WOControllableBall : public Aftr::WO
	{
	public:
		static WOControllableBall* New(physx::PxPhysics* physics, float radius);
		void onUpdateWO() override;

		std::string model = "";
		std::string metalTexture = "";
		std::string rubberTexture = "";

		physx::PxMaterial* materialMetal, * materialRubber;
		physx::PxRigidDynamic* thisActor;

		physx::PxShape* metalShape, * rubberShape;
		bool isShapeRubber = false;

		void SwitchMaterial();
		

		void setPosition(const Vector& newXYZ) override;
		void setPosition(float x, float y, float z) override;
	protected:
		void onCreate() override;
		WOControllableBall();
		~WOControllableBall() override;
		
	};
}