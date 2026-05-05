#pragma once

#include<Engine/Build-Options.h>
#include<Engine/API/def.h>
#include<Editor/API/def.h>

#include<cstdint>
#include<string>
#include<vector>
#include<map>
#include<unordered_map>
#include<memory>
#include<algorithm>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>
#endif

#if defined(PITAYA_WINDOW_GLFW)
#include<GLFW/glfw3.h>
#include<GLFW/glfw3native.h>  
#endif

#if defined(PITAYA_PHYSICS_BULLET)
#include<btBulletDynamicsCommon.h>
#include<BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include<BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include<BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>
#include<LinearMath/btThreads.h>
#endif
