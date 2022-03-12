#pragma once

#include "GLView.h"
#include <chrono>
#include "NetMessengerClient.h"

namespace physx
{
    class PxFoundation;
    class PxPhysics;
    class PxScene;
    class PxMaterial;
    class PxDefaultAllocator;
    class PxDefaultErrorCallback;
    class PxPvd;
    class PxRigidDynamic;
}

namespace Aftr
{
   class Camera;

    

/**
   \class GLViewNat22_Module7
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

   class WOControllableBall;

class GLViewNat22_Module7 : public GLView
{
public:
   static GLViewNat22_Module7* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewNat22_Module7();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createNat22_Module7WayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

   physx::PxDefaultAllocator* physAllocator;
   physx::PxDefaultErrorCallback* physErrCallback;
   physx::PxFoundation* physFoundation;
   physx::PxPhysics* physPhysics;
   physx::PxScene* physScene;
   physx::PxMaterial* physStaticMaterial;
   
   physx::PxPvd* physVisualDebugger;

   physx::PxRigidDynamic* cube;
   WO* cubeVis;

   std::chrono::system_clock::time_point deltaTime;

    //Client
   bool isClient =  true;
   WO* ClientObject;

   NetMessengerClient* client;

   WOControllableBall* ball;


protected:
   GLViewNat22_Module7( const std::vector< std::string >& args );
   virtual void onCreate();

};

/** \} */

} //namespace Aftr
