/*****************************************************************************
 *            Copyright (C) - InfinyTech3D - All Rights Reserved             *
 *                                                                           *
 * Unauthorized copying of this file, via any medium is strictly prohibited  *
 * Proprietary and confidential.                                             *
 *                                                                           *
 * Written by Erik Pernod <erik.pernod@infinytech3d.com>, January 2019       *
 ****************************************************************************/
#pragma once

// Self-contained vendored copy - deliberately does not include SofaVerseAPI/SofaPhysicsDefines.h
// (this plugin never needs the SAPAPI_* named error constants, just raw return-code comparisons),
// matching how the previous vendored header avoided pulling in the SOFA-core config chain.
#ifndef WIN32
#ifdef SOFA_BUILD_SOFAVERSEAPI
#	define SOFA_VERSE_API __attribute__ ((visibility ("default")))
#else
#   define SOFA_VERSE_API
#endif
#else
#ifdef SOFA_BUILD_SOFAVERSEAPI
#	define SOFA_VERSE_API __declspec( dllexport )
#else
#   define SOFA_VERSE_API __declspec( dllimport )
#endif
#   ifdef _MSC_VER
#       pragma warning(disable : 4231)
#       pragma warning(disable : 4910)
#   endif
#endif

#include <memory>
#include <string>

class SofaVerseAPI;

/// Curated public C++ facade over SofaVerseAPI - vendored copy for the SofaUE5 plugin, kept in
/// sync manually with sofa-plugins/SofaVerseAPI/src/SofaVerseAPI/SofaVerseBridge.h (the real,
/// canonical copy). Forwards to the private SofaVerseAPI engine and holds no logic of its own -
/// this is the only header this plugin should ever vendor from SofaVerseAPI going forward.
class SOFA_VERSE_API SofaVerseBridge
{
public:
    explicit SofaVerseBridge(int nbrThread = 1);
    ~SofaVerseBridge();

    SofaVerseBridge(const SofaVerseBridge&) = delete;
    SofaVerseBridge& operator=(const SofaVerseBridge&) = delete;

    const char* APIName();

    // --- Scene lifecycle ---
    int createScene();
    int load(const char* filename);
    int loadDefaultPlugins(const char* pluginPath);
    std::string loadSofaIni(const char* pathIni);
    void start();
    void step();
    void stop();

    // --- Simulation properties ---
    int getGravity(double* values) const;
    int setGravity(double* gravity);
    double getTimeStep() const;
    void setTimeStep(double dt);

    // --- Messages ---
    int activateMessageHandler(bool value);
    int getNbMessages();
    int getMessage_out(int messageId, std::string& outName);
    int clearMessages();

    // --- DAG node / component introspection ---
    int getNbrDAGNode();
    int getDAGNodeAPIName_out(int nodeID, std::string& outName);
    int getDAGNodeDisplayName_out(int nodeID, std::string& outName);
    int getDAGNodeParentAPIName_out(const std::string& nodeName, std::string& outName);
    int getNbrComponentsInNode(const std::string& nodeName);
    int getDAGNodeComponentName_out(const std::string& nodeName, int compoID, std::string& outName);
    int getComponentType_out(const std::string& componentName, std::string& outName);
    int getBaseComponentType_out(const std::string& componentName, std::string& outName);
    int getComponentDisplayName_out(const std::string& componentName, std::string& outName);

    // --- Mesh geometry ---
    int getNbVertices(const std::string& name);
    int getVPositions(const std::string& name, float* buffer);
    int getVNormals(const std::string& name, float* buffer);
    int getVTexCoords(const std::string& name, float* buffer);
    unsigned int getNbTriangles(const std::string& name);
    int getTriangles(const std::string& name, int* buffer);
    unsigned int getNbQuads(const std::string& name);
    int getQuads(const std::string& name, int* buffer);

private:
    std::unique_ptr<SofaVerseAPI> m_impl;
};
