#include "GLViewNat22_Module7.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVStaticBox.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include "include/PxPhysicsAPI.h"
#include "NetMsgUpdatePhysWO.h"
#include "ControllableBall.h"
#include "WOGridECEFElevation.h"

using namespace Aftr;

GLViewNat22_Module7* GLViewNat22_Module7::New( const std::vector< std::string >& args )
{
   GLViewNat22_Module7* glv = new GLViewNat22_Module7( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewNat22_Module7::GLViewNat22_Module7( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewNat22_Module7::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewNat22_Module7::onCreate() is invoked after this module's LoadMap() is completed.

    physAllocator = new physx::PxDefaultAllocator();
    physErrCallback = new physx::PxDefaultErrorCallback();

    physFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *physAllocator, *physErrCallback);
    physVisualDebugger = physx::PxCreatePvd(*physFoundation);

    physPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *physFoundation, physx::PxTolerancesScale(),
        false, physVisualDebugger);
    physx::PxSceneDesc sc(physPhysics->getTolerancesScale());
    sc.filterShader = physx::PxDefaultSimulationFilterShader;
    sc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

    physScene = physPhysics->createScene(sc);


    physScene->setGravity(physx::PxVec3(0, 0, -9.81f));

    client = NetMessengerClient::New();
}


void GLViewNat22_Module7::onCreate()
{
   //GLViewNat22_Module7::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here,
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1

   deltaTime = std::chrono::system_clock::now();


}


GLViewNat22_Module7::~GLViewNat22_Module7()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewNat22_Module7::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
    
   if (!isClient) {
       auto d = std::chrono::system_clock::now() - deltaTime;
       d = std::chrono::duration_cast<std::chrono::milliseconds>(d);
       float e = d.count();
       physScene->simulate((float)(e / 10000000));
       physScene->fetchResults(true);

       physx::PxMat44 mat = physx::PxMat44(cube->getGlobalPose());
       Mat4 afterMat;

       for (int i = 0; i < 16; i++)
       {
           afterMat[i] = mat(i % 4, i / 4);
       }
       cubeVis->setDisplayMatrix(afterMat);
       cubeVis->setPosition(Vector(cube->getGlobalPose().p.x, cube->getGlobalPose().p.y, cube->getGlobalPose().p.z));

       deltaTime = std::chrono::system_clock::now();


		// send netmsg
       {
           NetMsgUpdatePhysWO a;
           a.wo = cubeVis;
           a.xPos = cubeVis->getPosition().x;
           a.yPos = cubeVis->getPosition().y;
           a.zPos = cubeVis->getPosition().z;

           a.mat1_1 = cubeVis->getWO()->getDisplayMatrix().at(0);
           a.mat1_2 = cubeVis->getWO()->getDisplayMatrix().at(1);
           a.mat1_3 = cubeVis->getWO()->getDisplayMatrix().at(2);
           a.mat2_1 = cubeVis->getWO()->getDisplayMatrix().at(4);
           a.mat2_2 = cubeVis->getWO()->getDisplayMatrix().at(5);
           a.mat2_3 = cubeVis->getWO()->getDisplayMatrix().at(6);
           a.mat3_1 = cubeVis->getWO()->getDisplayMatrix().at(8);
           a.mat3_2 = cubeVis->getWO()->getDisplayMatrix().at(9);
           a.mat3_3 = cubeVis->getWO()->getDisplayMatrix().at(10);

           client->sendNetMsgSynchronousTCP(a);
       }
   }
}


void GLViewNat22_Module7::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewNat22_Module7::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewNat22_Module7::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewNat22_Module7::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewNat22_Module7::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if( key.keysym.sym == SDLK_1 )
   {
       ball->SwitchMaterial();
   }

    if (key.keysym.sym == SDLK_2)
    {
        ball = WOControllableBall::New(physPhysics, 1);
        ball->thisActor->getGlobalPose().p = physx::PxVec3(30, 0, 50);
        worldLst->push_back(ball);
    }
}


void GLViewNat22_Module7::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewNat22_Module7::loadMap()
{

   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( true ); //set to TRUE to enable shadow mapping, must be using GL 3.2

   this->cam->setPosition( 15,15,10 );

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );


   {
      //Create a light
      float ga = 0.1f; //Global Ambient Light level for this module
      ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
      WOLight* light = WOLight::New();
      light->isDirectionalLight( true );
      light->setPosition( Vector( 0, 0, 100 ) );
      //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
      //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
      light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
      light->setLabel( "Light" );
      worldLst->push_back( light );
   }

   {
      //Create the SkyBox
      WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
      wo->setPosition( Vector( 0, 0, 0 ) );
      wo->setLabel( "Sky Box" );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      worldLst->push_back( wo );
   }

   { 
      if (!isClient)
      {
          using namespace physx;
          physStaticMaterial = physPhysics->createMaterial(1, 1, 1);

        PxMaterial* physBoxMaterial = physPhysics->createMaterial(1, 1, 1);


          physx::PxRigidStatic* grassPlane = physx::PxCreatePlane(*physPhysics, physx::PxPlane(0, 0, 1, 0   ), *physStaticMaterial);
          physScene->addActor(*grassPlane);
        
          cube = physPhysics->createRigidDynamic(PxTransform(0, 0, 20));
          cube->attachShape(*physPhysics->createShape(PxBoxGeometry(2, 2, 2), *physBoxMaterial));
          physScene->addActor(*cube);

          cubeVis = WO::New(shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT);
          //cubeVis->setLabel("PhysWO");
          cubeVis->setPosition(0, 0, 20);
          worldLst->push_back(cubeVis);

          
          std::cout << "start creating terrain\n";
          float top = 41.1875f;
          float bottom = 41.125f;
          float left = -96.0625f;
          float right = -96.0f;

          float vert = top - bottom;
          float horz = right - left;

          VectorD offset((top + bottom) / 2, (left + right) / 2, 0);
          VectorD upperLeft(top, left, 0);
          VectorD lowerRight(bottom, right, 0);
          VectorD oScl(1, 1, 1);
          WO* o;
          //change to tid.tif for custom.
          o = WOGridECEFElevation::New(upperLeft, lowerRight, 0, offset, oScl, ManagerEnvironmentConfiguration::getLMM() + "/models/USGSSync.tif");
          o->setLabel("terrain");
          o->setPosition(0, 0, 0);
          worldLst->push_back(o);
      }
      else
      {
          ClientObject = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
          //cubeVis->setLabel("ClientWO");
          ClientObject->setPosition(0, 0, 20);
          worldLst->push_back(ClientObject);
      }


   }
   
   //Make a Dear Im Gui instance via the WOImGui in the engine... This calls
   //the default Dear ImGui demo that shows all the features... To create your own,
   //inherit from WOImGui and override WOImGui::drawImGui_for_this_frame(...) (among any others you need).
   WOImGui* gui = WOImGui::New( nullptr );
   gui->setLabel( "My Gui" );
   gui->subscribe_drawImGuiWidget(
      [this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
      {
         ImGui::ShowDemoWindow(); //Displays the default ImGui demo from C:/repos/aburn/engine/src/imgui_implot/implot_demo.cpp
         WOImGui::draw_AftrImGui_Demo( gui ); //Displays a small Aftr Demo from C:/repos/aburn/engine/src/aftr/WOImGui.cpp
         ImPlot::ShowDemoWindow(); //Displays the ImPlot demo using ImGui from C:/repos/aburn/engine/src/imgui_implot/implot_demo.cpp
      } );
   this->worldLst->push_back( gui );

   createNat22_Module7WayPoints();
}


void GLViewNat22_Module7::createNat22_Module7WayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWayPointSpherical::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}
