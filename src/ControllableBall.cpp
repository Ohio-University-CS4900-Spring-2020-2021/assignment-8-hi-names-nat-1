#include "ControllableBall.h"

#include <Model.h>

#include "GLViewNat22_Module7.h"
#include "WO.h"
#include "include/PxPhysicsAPI.h"
#include "Mat4.h"
#include "ManagerEnvironmentConfiguration.h"
namespace Aftr
{
	class GLViewNat22_Module7;

	Aftr::WOControllableBall* WOControllableBall::New(physx::PxPhysics* physics, float radius)
	{
		WOControllableBall* ball = new WOControllableBall();
		ball->onCreate();
		ball->thisActor = physics->createRigidDynamic(physx::PxTransform(30, 0,30));
		ball->materialMetal = physics->createMaterial(.8f, .8f, .1f);
		ball->materialRubber = physics->createMaterial(.2f, .1f, .7f);
		ball->metalShape = physics->createShape(physx::PxSphereGeometry(radius), *ball->materialMetal);
		ball->rubberShape = physics->createShape(physx::PxSphereGeometry(radius), *ball->materialRubber);
		ball->thisActor->attachShape(*ball->metalShape);
		ManagerGLView::getGLViewT<GLViewNat22_Module7>()->physScene->addActor(*ball->thisActor);

		ball->setPosition(20, 0, 50);
		return ball;
		
	}

	void WOControllableBall::onUpdateWO()
	{
		physx::PxMat44 mat = physx::PxMat44(thisActor->getGlobalPose());
		Mat4 afterMat;

		for (int i = 0; i < 16; i++)
		{
			afterMat[i] = mat(i % 4, i / 4);
		}
		this->setDisplayMatrix(afterMat);
		this->setPosition(Vector(thisActor->getGlobalPose().p.x, thisActor->getGlobalPose().p.y, thisActor->getGlobalPose().p.z));
	}

	void WOControllableBall::setPosition(const Vector& newXYZ)
	{
		WO::setPosition(newXYZ);
	}

	void WOControllableBall::setPosition(float x, float y, float z)
	{
		setPosition(Vector(x, y, z));
	}




	void WOControllableBall::onCreate()
	{
		rubberTexture = ManagerEnvironmentConfiguration::getLMM() + "/models/rubber.png";
		metalTexture = ManagerEnvironmentConfiguration::getLMM() + "/models/metal.jpg";
		model = ManagerEnvironmentConfiguration::getLMM() + "/models/ball.dae";


		WO::onCreate(model, Vector(1, 1, 1));
		upon_async_model_loaded([this]()
			{
				getModel()->addSkin(metalTexture, 0);
				getModel()->getSkins().at(0).setMeshShadingType(MESH_SHADING_TYPE::mstSMOOTH);
				getModel()->addSkin(rubberTexture, 0);
				getModel()->getSkins().at(1).setMeshShadingType(MESH_SHADING_TYPE::mstSMOOTH);

				//for (int i = 0; i < getModel()->getSkins().size(); i++)
				//{
				//	ModelMeshSkin& skin = getModel()->getModelDataShared()->getModelMeshes().at(i)->getSkins().at(0);
				//	skin.getMultiTextureSet().at(0)->setTextureRepeats(5.0f);
				//	skin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Color of object when it is not in any light
				//	skin.setDiffuse(aftrColor4f(.0f, 5.0f, 5.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
				//	skin.setSpecular(aftrColor4f(0.9f, 0.9f, 0.9f, 1.0f)); //Specular color component (ie, how "shiney" it is)
				//	skin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
				//}

			});
		getModel()->useSkinAtIndex(1);
		//setPosition(0, 0, 0);

		
	}

	WOControllableBall::WOControllableBall() : WO(), IFace(this)
	{
		
		
	}

	void WOControllableBall::SwitchMaterial()
	{
		if (this == nullptr) return;
		if (!isShapeRubber)
		{
			thisActor->detachShape(*metalShape);
			thisActor->attachShape(*rubberShape);
			isShapeRubber = true;
			std::cout << "Updated pxmaterial to rubber\n";
			getModel()->useSkinAtIndex(1);
		}
		else
		{
			thisActor->detachShape(*rubberShape);
			thisActor->attachShape(*metalShape);
			isShapeRubber = false;
			std::cout << "Updated pxmaterial to metal\n";
			getModel()->useSkinAtIndex(0);

		}
	}


	WOControllableBall::~WOControllableBall()
	{
		thisActor->release();
	}
	
}