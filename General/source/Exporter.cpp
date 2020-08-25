#include "Exporter.h"

using json = nlohmann::json;

const char* const TGeoManagerExporter::proxy_type = "solid.proxy";
const char* const TGeoManagerExporter::composite_type = "solid.composite";
const char* const TGeoManagerExporter::group_type = "group.solid";

const char* const TGeoManagerExporter::k_templates_name = "prototypes";
const char* const TGeoManagerExporter::k_children_name = "children";
const char* const TGeoManagerExporter::k_styles_name = "styleSheet";
const char* const TGeoManagerExporter::k_properties_name = "properties";
const char* const TGeoManagerExporter::k_template_field = "templateName";

json TGeoManagerExporter::convertComposite(TGeoCompositeShape *composite) {
    json j = {};
    j["type"] = composite_type;
    auto bool_node = composite->GetBoolNode();

    json first_pos = convertMatrix(bool_node->GetLeftMatrix());
    json first_shape = convertShape(bool_node->GetLeftShape());

    j["first"] = json_union(first_shape, first_pos);
    //first_shape.merge_patch(first_pos);
    //j["first"] = first_shape;

    json second_pos = convertMatrix(bool_node->GetRightMatrix());
    json second_shape = convertShape(bool_node->GetRightShape());

    j["second"] = json_union(second_shape, second_pos);
    //second_shape.merge_patch(second_pos);
    //j["second"] = second_shape;

    std::string operator_type;
    switch (bool_node->GetBooleanOperator()) {
        case TGeoBoolNode::EGeoBoolType::kGeoIntersection:
            operator_type = "INTERSECT";
            break;
        case TGeoBoolNode::EGeoBoolType::kGeoSubtraction:
            operator_type = "SUBTRACT";
            break;
        case TGeoBoolNode::EGeoBoolType::kGeoUnion:
            operator_type = "UNION";
    }
    j["compositeType"] = operator_type;
    return j;
}

json TGeoManagerExporter::convertStyles() const {
    json styleSheet;

    for (auto material : materials) {
        json j;
        j["gdml"]["material"] = material->GetName();

        TColor* color = gROOT->GetColor(material->GetDefaultColor());
        auto opacity = (int64_t)material->GetTransparency();

        if (opacity != 0) {
            j["material"]["opacity"] = (int64_t)material->GetTransparency();
        }
        j["material"]["color"] = stringFromColor(color);

        std::string material_entry = "material[" + std::string(material->GetName()) + "]";

        styleSheet[material_entry] = j;
    }

    return styleSheet;

}

std::string TGeoManagerExporter::stringFromColor(TColor *color) const { 
    uint16_t r = (int)(color->GetRed() * 255);
    uint16_t g = (int)(color->GetGreen() * 255);
    uint16_t b = (int)(color->GetBlue() * 255);

    std::stringstream sstr;
    sstr << "#" << std::hex << (r << 16 | g << 8 | b );
    return sstr.str();
}

json TGeoManagerExporter::convertTemplates() const {
    return json_union(convertTemplateVolumes(), convertTemplateNodes());
}

json TGeoManagerExporter::convertTemplateVolumes() const {
    json j;

    for (auto volume_entry : volumes2) {
        auto volume = volume_entry.second;
        if (volume == nullptr) {
            continue;
        }
        j[volume->GetName()] = convertShape(volume->GetShape());

        auto material = volume->GetMaterial();
        if (material != nullptr) {   
            std::string material_entry = getMaterialEntry(material);
            j[volume->GetName()][k_properties_name] = {{"@style", material_entry}};
        }
    }
    return j;
}

json TGeoManagerExporter::convertTemplateNodes() const {
    json j;
    j["type"] = group_type;
    for (auto node_entry : nodes2) {
        auto node = node_entry.second;

        j[k_children_name][node->GetName()]["position"] = convertMatrix(node->GetMatrix());
        j[k_children_name][node->GetName()]["type"] = group_type;

        j[k_children_name][node->GetName()][k_children_name][node->GetVolume()->GetName()]["type"] = proxy_type;
        j[k_children_name][node->GetName()][k_children_name][node->GetVolume()->GetName()][k_template_field] = "volumes." + std::string(node->GetVolume()->GetName());

        if (node->GetNodes() != nullptr && node->GetNodes()->GetSize() > 0) {
            for (auto obj : *(node->GetNodes())) {
                auto nd = dynamic_cast<TGeoNode*> (obj);
                j[k_children_name][node->GetName()][k_children_name] = json_union(convertChildTemplateNode(nd), j[k_children_name][node->GetName()][k_children_name]);
            }    
        }
    }  

    json j_nodes;
    j_nodes["volumes"] = j; 
    return j_nodes;      
}

json TGeoManagerExporter::convertChildTemplateNode(TGeoNode* node) const {
    json j;
    j[node->GetName()] = {
        {"type", proxy_type},
        {k_template_field, node->GetName()}
    };

    return j;
}

std::string TGeoManagerExporter::getMaterialEntry(TGeoMaterial* material) const {
    return "material[" + std::string(material->GetName()) + "]";
}

json TGeoManagerExporter::convert(TGeoManager* _geomanager) {

    json j;

    prepare(_geomanager);

    j["type"] = group_type;

    j[k_styles_name] = convertStyles();

    j[k_templates_name] = convertTemplates();

    j[k_properties_name] = convertProperties();

    j[k_children_name] = convertChildren();

    return j;
}

json TGeoManagerExporter::convertProperties() const {
    json j;
    j["rotation"] = {{"order", "ZXY"}};
    return j;
}

json TGeoManagerExporter::convertChildren() const {
    return convertNode(gGeoManager->GetTopNode());
}

json TGeoManagerExporter::convertNode(TGeoNode* node) const {
    json j;
    if(node == nullptr) {
        return j;
    }

    if (node->GetVolume()) {
        j[node->GetName()]["type"] = proxy_type;
        j[node->GetName()][k_template_field] = node->GetName();
    // } else {
    //     wr.AddProperty("type", "ERROR");
    }

    auto matrix = node->GetMatrix();
    if (matrix != nullptr) {
        j = json_union(convertMatrix(matrix), j);
    }
    return j;
}

void TGeoManagerExporter::prepare(TGeoManager* _geomanager) {
    geoManager = _geomanager;
    TList* materials_list = geoManager->GetListOfMaterials();
    TIter next(materials_list);
    TGeoMaterial *material;
    while ((material = (TGeoMaterial *)next())) {
        materials.insert(material);
    }
    std::queue<TGeoVolume*> volume_que;
    volume_que.push(geoManager->GetMasterVolume());
    nodes.insert(gGeoManager->GetTopNode());
    nodes2[gGeoManager->GetTopNode()->GetName()] = gGeoManager->GetTopNode();

    while(!volume_que.empty()) {
        TGeoVolume* current_volume = volume_que.front();
        volume_que.pop();
        volumes.insert(current_volume);
        materials.insert(current_volume->GetMaterial());
        volumes2[current_volume->GetName()] = current_volume;

        TObjArray* current_nodes = current_volume->GetNodes();
        if (current_nodes == nullptr) {
            continue;
        }
        for (int i = 0; i < current_nodes->GetSize(); ++i) {
            auto node = (TGeoNode*)current_nodes->At(i);
            if (node == nullptr) {
                continue;
            }
            nodes.insert(node);
            nodes2[node->GetName()] = node;
            if (volumes.find(node->GetVolume()) == volumes.end()) {
                volume_que.push(node->GetVolume());
            }
        }
    }
}

// OTHER 

double TGeoManagerExporter::DegreeToRad(double angle) {  //!!!!!!
    constexpr double Pi = 3.14159265;
    return angle * Pi / 180;
}

json TGeoManagerExporter::json_union(const json &j1, const json &j2) {
    json j = j1;
    if (!j2.empty()) {
        j.merge_patch(j2);
    }
    return j;
}

