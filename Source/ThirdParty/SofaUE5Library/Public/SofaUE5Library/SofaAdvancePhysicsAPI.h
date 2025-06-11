/*****************************************************************************
 *            Copyright (C) - InfinyTech3D - All Rights Reserved             *
 *                                                                           *
 * Unauthorized copying of this file, via any medium is strictly prohibited  *
 * Proprietary and confidential.                                             *
 *                                                                           *
 * Written by Erik Pernod <erik.pernod@infinytech3d.com>, January 2019       *
 ****************************************************************************/
#pragma once

#ifndef WIN32
#ifdef SOFA_BUILD_SOFAVERSEAPI
#	define SOFA_VERSE_API __attribute__ ((visibility ("default")))
#else
#   define SOFA_VERSE_API
#endif
#else
#ifdef SOFA_BUILD_SOFAPHYSICSAPI
#	define SOFA_VERSE_API __declspec( dllexport )
#else
#   define SOFA_VERSE_API __declspec( dllimport )
#endif
#   ifdef _MSC_VER
#       pragma warning(disable : 4231)
#       pragma warning(disable : 4910)
#   endif
#endif


//#include <SofaVerseAPI/SofaPhysicsDefines.h>
#include <string>
#include <vector>
#include <memory>

class SofaPhysicsObjectsManager;
class SofaPhysicsInteractionsManager;
class SofaPliersManager;
class SofaEntactManager;
class SofaGeomagicManager;
class SofaHaplyRoboticsManager;
class SofaKeyEventManager;

class SofaGraphicManager;
class SofaRenderEvent;
using SofaRenderEventPtr = std::shared_ptr<SofaRenderEvent>;

namespace sofaVerseAPI
{
    class SofaDAGNodesManager;
    class SofaComponentsManager;
    class SofaSimulationManager;
    class SofaBaseComponentAPI;

    class SofaBaseMeshAPI;
}


typedef unsigned int Index; ///< Type used for topology indices
typedef void* ID;           ///< Type used for IDs

class SOFA_VERSE_API SofaAdvancePhysicsAPI
{
public:
    SofaAdvancePhysicsAPI(int nbrThread = 1);
    virtual ~SofaAdvancePhysicsAPI();    
    
    const char* APIName();
    const std::string& getAPIName();
    void setAPIName(const std::string& name);

    ////////////////////////////////////////////
    //////         Simulation API          /////
    ////////////////////////////////////////////
  
    /// Load an XML file containing the main scene description
	int load(const char* filename);
    int loadPlugin(const char* pluginName);
    int loadDefaultPlugins(const char* pluginPath);
    std::string loadSofaIni(const char* pathIni);
    int initGLContext();
    int freeGLContext();

    int createScene();

    /// Start the simulation
    /// Currently this simply sets the animated flag to true, but this might
    /// start a separate computation thread in a future version
    void start();

    /// Stop/pause the simulation
    void stop();

    /// Compute one simulation time-step
    void step();

    int unload();

    /// Compute one simulation time-step
    bool asyncStep();

    /// Query the physics simulation for the asynch step completion
    bool isAsyncStepCompleted();

    /// Reset the simulation to its initial state
    void reset();

    /// Return true if the simulation is running
    /// Note that currently you must call the step() method
    /// periodically to actually animate the scene
    bool isAnimated() const;

    /// Set the animated state to a given value (requires a
    /// simulation to be loaded)
    void setAnimated(bool val);

    /// Return the main simulation file name (from the last
    /// call to load())
    const char* getSceneFileName() const;

    /// Return the current time-step (or 0 if no simulation
    /// is loaded)
    double getTimeStep() const;
    /// Control the timestep of the simulation (requires a
    /// simulation to be loaded)
    void   setTimeStep(double dt);

    /// Return the current simulated time
    double getTime() const;

    int getStateMachine() const;

    /// Return the current computation speed (averaged over
    /// the last 20 steps)
    float getCurrentFPS() const;

    int getGravity(double* values) const;
    int setGravity(double* gravity);

    int activateMessageHandler(bool value);
    int getNbMessages();
    std::string getMessage(int messageId, int& msgType);
    int clearMessages();

    ////////////////////////////////////////////
    //////       MeshInterface API         /////
    ////////////////////////////////////////////

    int getNumberOfMeshIO();
    int getMeshIO(const std::string& name, sofaVerseAPI::SofaBaseMeshAPI* &meshIO);
    int getMeshIOUniqId(const std::string& name);
    int setMeshIOActive(const std::string& name, bool active);

    int getNbVertices(const std::string& name);
    int getVPositions(const std::string& name, float* buffer);  ///< vertices positions (Vec3)
    int getVNormals(const std::string& name, float* buffer);    ///< vertices normals   (Vec3)
    int getVTexCoords(const std::string& name, float* buffer);

    int getVVelocities(const std::string& name, float* buffer);  ///< vertices positions (Vec3)
    int getVForces(const std::string& name, float* buffer);  ///< vertices forces (Vec3)

    unsigned int getNbEdges(const std::string& name); ///< number of edges
    int getEdges(const std::string& name, int* buffer);   ///< edges topology (2 indices / triangle)	

	unsigned int getNbTriangles(const std::string& name); ///< number of triangles
	int getTriangles(const std::string& name, int* buffer);   ///< triangles topology (3 indices / triangle)	

	unsigned int getNbQuads(const std::string& name); ///< number of quads
	int getQuads(const std::string& name, int* buffer);   ///< quads topology (4 indices / quad)
    														  /// API to get tetrahedra
	unsigned int getNbTetrahedra(const std::string& name);
	int getTetrahedra(const std::string& name, int* buffer);

	unsigned int getNbHexahedra(const std::string& name);
	int getHexahedra(const std::string& name, int* buffer);

    int getTopologyRevision(const std::string& name);
    int setTopologyChanged(const std::string& name, bool value);

    int getMeshDimension(const std::string& name);

    int setNbVertices(const std::string& name, int nbrV);
    int setVertices(const std::string& name, float* values);
    int setVelocities(const std::string& name, float* values);    
    int setRestPositions(const std::string& name, float* values);
    
    ////////////////////////////////////////////
    //////      SOFA Components  API       /////
    ////////////////////////////////////////////

    int logSceneGraph();
    int getNbrDAGNode();
    std::string getDAGNodeAPIName(int nodeID);
    std::string getDAGNodeDisplayName(int nodeID);

    std::string getDAGNodeComponentsName(const std::string& nodeName);
    std::string getDAGNodeParentAPIName(const std::string& nodeName);
    
    std::string getBaseComponentTypes();
    std::string getBaseComponentType(const std::string& componentName);

    std::string getPossibleTypes(const std::string& componentName);
    std::string getComponentType(const std::string& componentName);
    std::string getComponentDisplayName(const std::string& componentName);

    int addDAGNode(const std::string& _DAGName, const std::string& _parentName);
    int changeDAGNodeAPIName(const std::string& _oldDAGName, const std::string& _newDAGName);

    int getDAGNodeTransform(const std::string& nodeName, const std::string& transformType, float* values);
    int setDAGNodeTransform(const std::string& nodeName, const std::string& transformType, float* values);

    int addBaseComponent(const std::string& componentName, const std::string& componentBaseType, const std::string& ownerNodeName);
    int setComponentType(const std::string& componentName, const std::string& componentType);

    std::string getDataFields(const std::string& componentName);
    std::string getLinks(const std::string& componentName);
    
    sofaVerseAPI::SofaBaseComponentAPI* getBaseComponent(const std::string& componentName, int& res);
    sofaVerseAPI::SofaBaseComponentAPI* getBaseComponentStr(const std::string& componentName, std::string& res);

    int reinitComponent(const std::string& componentName);

    ////////////////////////////////////////////
    //////       SOFA Objects  API         /////
    ////////////////////////////////////////////
    int addCollisionPipeline();

    int addCube(const std::string &name, const std::string & parentName, bool isRigid);
    int addCylinder(const std::string &name, const std::string & parentName, bool isRigid);
    int addPlane(const std::string &name, const std::string & parentName, bool isRigid);
    int addSphere(const std::string &name, const std::string & parentName, bool isRigid);

    int addSphereCollisionsObject(const std::string &name, std::string parentName);

    ////////////////////////////////////////////
    //////      Interaction mgrs  API      /////
    ////////////////////////////////////////////

    int createKeyPressEvent(int keyId);
    int createKeyReleaseEvent(int keyId);

    int createResectionTool(const std::string& name, float length);
	int createAttachTool(const std::string& name, float length);
	int createFixConstraintTool(const std::string& name, float length);

	int castRay(const std::string& name, float * origin, float * direction);
	int activateTool(const std::string& name, bool value);
    int setToolAttribute(const std::string& name, const std::string& dataName, float* value);
    const std::string& getInteractObjectName(const std::string& name);
    	
    int createPliers(const std::string& pliersName, const std::string& nameMord1, const std::string& nameMord2, const std::string& nameModel, float _stiffness);    

	int unactivePliers(const std::string& pliersName);
	int reactivePliers(const std::string& pliersName);
    int closePliers(const std::string& pliersName);
    int releasePliers(const std::string& pliersName);
    int idGrabed(const std::string& pliersName, int* ids);

    int createEntactManager(const std::string& toolName);
    int numberOfTools(const std::string& toolName);

    int createGeomagicManager(const std::string& toolName);
    int initGeomagicDevice(const std::string& toolName);
    int getGeomagicPosition(const std::string& toolName, float *values);
    int getGeomagicStatus(const std::string& toolName, int *value);
    int getGeomagicButtonStatus1(const std::string& toolName, int *value);
    int getGeomagicButtonStatus2(const std::string& toolName, int *value);

    int createInverse3Manager(const std::string& toolName);
    int getInverse3Position(const std::string& toolName, float* values);
    int getInverse3Status(const std::string& toolName, int* value);
    int getInverse3ButtonStatus(const std::string& toolName, int* value);

    int sofaPhysicsAPI_setPliersAxis(const std::string& pliersName, float * xAxis, float * yAxis, float * zAxis);
    int sofaPhysicsAPI_cutPliers(const std::string& pliersName, float *origin,
        float * xAxis, float * yAxis, float * zAxis, float length);

    int sofaPhysicsAPI_pathCutPliers(const std::string& pliersName, float *origin,
        float * xAxis, float * yAxis, float * zAxis, float length);

       
    ////////////////////////////////////////////
    //////       SOFA bindings API         /////
    ////////////////////////////////////////////

    // Data api bindings
    int getDataCounter(const std::string& componentName, const std::string& dataName, int* value);
    int getDataFlags(const std::string& componentName, const std::string& dataName, int* value);

    // basic types bindings
    int getBoolValue(const std::string& componentName, const std::string& dataName, bool* value);
    int setBoolValue(const std::string& componentName, const std::string& dataName, bool value);

    int getIntValue(const std::string& componentName, const std::string& dataName, int* value);
    int setIntValue(const std::string& componentName, const std::string& dataName, int value);

    int getUIntValue(const std::string& componentName, const std::string& dataName, unsigned int* value);
    int setUIntValue(const std::string& componentName, const std::string& dataName, unsigned int value);

    int getFloatValue(const std::string& componentName, const std::string & dataName, float* value);
    int setFloatValue(const std::string& componentName, const std::string& dataName, float value);

    int getDoubleValue(const std::string& componentName, const std::string & dataName, double* value);
    int setDoubleValue(const std::string& componentName, const std::string& dataName, double value);

    int getSRealValue(const std::string& componentName, const std::string & dataName, bool convertDouble, float* value);
    int setSRealValue(const std::string& componentName, const std::string& dataName, bool convertDouble, float value);

    std::string getStringValue(const std::string& componentName, const std::string& dataName);
    int setStringValue(const std::string& componentName, const std::string& dataName, const std::string& value);


    // Vec2 bindings
    int getVec2iValue(const std::string& componentName, const std::string & dataName, int * values);
    int setVec2iValue(const std::string& componentName, const std::string & dataName, int * values);

    int getVec2fValue(const std::string& componentName, const std::string& dataName, float* values);
    int setVec2fValue(const std::string& componentName, const std::string& dataName, float* values);

    int getVec2dValue(const std::string& componentName, const std::string& dataName, double* values);
    int setVec2dValue(const std::string& componentName, const std::string& dataName, double* values);

    int getVec2Value(const std::string& componentName, const std::string& dataName, bool convertDouble, float* values);
    int setVec2Value(const std::string& componentName, const std::string& dataName, bool convertDouble, float* values);


    // Vec3 bindings
    int getVec3iValue(const std::string& componentName, const std::string & dataName, int * values);
    int setVec3iValue(const std::string& componentName, const std::string & dataName, int * values);

    int getVec3fValue(const std::string& componentName, const std::string& dataName, float* values);
    int setVec3fValue(const std::string& componentName, const std::string& dataName, float* values);

    int getVec3dValue(const std::string& componentName, const std::string& dataName, double* values);
    int setVec3dValue(const std::string& componentName, const std::string& dataName, double* values);

    int getVec3Value(const std::string& componentName, const std::string& dataName, bool convertDouble, float* values);
    int setVec3Value(const std::string& componentName, const std::string& dataName, bool convertDouble, float* values);


    // Vec4 bindings
    int getVec4fValue(const std::string& componentName, const std::string& dataName, float* values);
    int setVec4fValue(const std::string& componentName, const std::string& dataName, float* values);

    int getVec4Value(const std::string& componentName, const std::string& dataName, bool convertDouble, float* values);
    int setVec4Value(const std::string& componentName, const std::string& dataName, bool convertDouble, float* values);


    // Rigid3 bindings
    int getRigid3fValue(const std::string& componentName, const std::string& dataName, float* values);
    int setRigid3fValue(const std::string& componentName, const std::string& dataName, float* values);

    int getRigid3dValue(const std::string& componentName, const std::string& dataName, double* values);
    int setRigid3dValue(const std::string& componentName, const std::string& dataName, double* values);

    int getRigid3Value(const std::string& componentName, const std::string& dataName, bool convertDouble, float* values);
    int setRigid3Value(const std::string& componentName, const std::string& dataName, bool convertDouble, float* values);


    // Vector bindings
    int getVectorSize(const std::string& componentName, const std::string& dataName, const std::string& dataType, int * value);

    int getVectorValue(const std::string& componentName, const std::string& dataName, const std::string& dataType, int size, float* values);
    int setVectorValue(const std::string& componentName, const std::string& dataName, const std::string& dataType, int size, float* values);

    int getVectorfValue(const std::string& componentName, const std::string& dataName, int size, float* values);
    int setVectorfValue(const std::string& componentName, const std::string& dataName, int size, float* values);

    int getVectordValue(const std::string& componentName, const std::string& dataName, int size, double* values);
    int setVectordValue(const std::string& componentName, const std::string& dataName, int size, double* values);

    int getVectoriValue(const std::string& componentName, const std::string& dataName, int size, int* values);
    int setVectoriValue(const std::string& componentName, const std::string& dataName, int size, int* values);

    // Vector of Vec2 bindings
    int getVecOfVec2Size(const std::string& componentName, const std::string& dataName, bool doubleValue, int* value);
    int getVecOfVec2Value(const std::string& componentName, const std::string& dataName, int size, bool doubleValue, float* values);
    int setVecOfVec2Value(const std::string& componentName, const std::string& dataName, int size, bool doubleValue, float* values);


    // Vector of Vec3 bindings
    int getVecOfVec3Size(const std::string& componentName, const std::string& dataName, bool doubleValue, int* value);
    int getVecOfVec3Value(const std::string& componentName, const std::string& dataName, int size, bool doubleValue, float* values);
    int setVecOfVec3Value(const std::string& componentName, const std::string& dataName, int size, bool doubleValue, float* values);

    int getVecOfVec3dSize(const std::string& componentName, const std::string& dataName, int* value);
    int getVecOfVec3dValue(const std::string& componentName, const std::string& dataName, int size, double* values);
    int setVecOfVec3dValue(const std::string& componentName, const std::string& dataName, int size, double* values);

    ////////////////////////////////////////////
    //////      Graphic mgr  API           /////
    ////////////////////////////////////////////
    int clearUp();
    int onRenderEvent(int eventID);
    int registerRenderEvent(SofaRenderEventPtr event);

protected:    
    sofaVerseAPI::SofaDAGNodesManager* m_sofaNodeManager = nullptr;
    /// Internal implementation sub-class
    sofaVerseAPI::SofaSimulationManager* m_simulationMgr;
    sofaVerseAPI::SofaComponentsManager* m_sofaComponentManager = nullptr;

    SofaPhysicsObjectsManager* m_3DObjectManager = nullptr;
    SofaPhysicsInteractionsManager* m_3DInteractionsManager = nullptr;

    SofaPliersManager* m_pliersManager = nullptr;
    SofaEntactManager* m_entactManager = nullptr;
    SofaGeomagicManager* m_geomagicManager = nullptr;
    SofaHaplyRoboticsManager* m_haplyManager = nullptr;
    SofaKeyEventManager* m_keyEventManager = nullptr;

    SofaGraphicManager* m_graphicManager = nullptr;
        
	std::string m_APIName;

    int m_stateMachine;
};
