#ifndef TEST_FOO
#define TEST_FOO

#include <TGeoManager.h>
#include <nlohmann/json.hpp>

#include <TColor.h>
#include <TGeoBBox.h>
#include <TGeoBoolNode.h>
#include <TGeoCompositeShape.h>
#include <TGeoCone.h>
#include <TGeoManager.h>
#include <TGeometry.h>
#include <TGeoNode.h>
#include <TGeoSphere.h>
#include <TGeoTube.h>
#include <TGeoPara.h>
#include <TGeoArb8.h>
#include <TGeoXtru.h>
#include <TObject.h>
#include <TROOT.h>

#include <iostream>
#include <ostream>
#include <queue>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;
//using namespace std;

class TGeoManagerExporter {
private:
    TGeoManager* geoManager;

    static const char *const proxy_type;
    static const char *const composite_type;
    static const char *const group_type;

    static const char *const k_templates_name;
    static const char *const k_children_name;
    static const char *const k_styles_name;
    static const char *const k_properties_name;
    static const char *const k_template_field;

    std::unordered_set<TGeoVolume*> volumes;
    std::unordered_set<TGeoNode*> nodes;
    std::unordered_map<std::string, TGeoVolume*> volumes2;
    std::unordered_map<std::string, TGeoNode*> nodes2;
    std::unordered_set<TGeoMaterial*> materials;

    static json convertShape(TGeoShape* shape);
    static json convertComposite(TGeoCompositeShape* composite);
    static json convertTube(TGeoTube* tube);
    static json convertBox(TGeoBBox* shape);
    static json convertCone(TGeoCone* shape);
    static json convertSphere(TGeoSphere* sphere);
    static json convertXtru(TGeoXtru *xtru);

    static json convertMatrix(TGeoMatrix* matrix);
    static json convertGeneralMatrix(TGeoGenTrans* matrix);
    static json convertCombination(TGeoCombiTrans* matrix);
    static json convertRotation(TGeoRotation* matrix);
    static json convertTranslation(TGeoTranslation* matrix);
    static json convertScale(TGeoScale* matrix);
    static json convertRotationBlock(double phi, double theta, double psi);
    static json convertTranslationBlock(double x, double y, double z);
    static json convertScaleBlock(double x, double y, double z);

    json convertStyles() const;

    static double DegreeToRad(double angle);
    static json json_union(const json &j1, const json &j2);
    std::string stringFromColor(TColor *color) const;
    std::string getMaterialEntry(TGeoMaterial* material) const;

    json convertTemplates() const;
    json convertTemplateVolumes() const;
    json convertTemplateNodes() const;
    json convertChildTemplateNode(TGeoNode* node) const;
    json convertProperties() const;
    json convertChildren() const;
    json convertNode(TGeoNode* node) const;

    void prepare(TGeoManager* _geomanager);

public:
    json convert(TGeoManager* _geomanager);

};


#endif