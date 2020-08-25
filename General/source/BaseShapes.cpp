#include "Exporter.h"

json TGeoManagerExporter::convertShape(TGeoShape *shape) {
    json j = {};
    if (shape == nullptr) {
        return j;
    }
    if (shape->TestShapeBit(TGeoCompositeShape::EShapeType::kGeoComb)) {
        return convertComposite(dynamic_cast<TGeoCompositeShape*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoCone)) {
        return convertCone(dynamic_cast<TGeoCone*>(shape));
    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoTube)) {
        return convertTube(dynamic_cast<TGeoTube*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoSph)) {
        return convertSphere(dynamic_cast<TGeoSphere*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoBox)) {
        return convertBox(dynamic_cast<TGeoBBox*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoXtru)) {
        return convertXtru(dynamic_cast<TGeoXtru*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoBad) ||
              shape->TestShapeBit(TGeoShape::EShapeType::kGeoNoShape)) {
        j["type"] = "ErrorType";
        return j;
    } else {
        j["type"] = "Unknown";
        return j;
    }
}

json TGeoManagerExporter::convertTube(TGeoTube *tube) {
    json j;
    j["type"] = "solid.tube";

    if (tube->GetRmin() != 0.0) {
        j["innerRadius"] = tube->GetRmin();
    }
    if (tube->GetRmax() != 0.0) {
        j["radius"] = tube->GetRmax();
    }
    if (tube->GetDz() != 0.0) {
        j["height"] = tube->GetDz() * 2.0;
    }

    return j;
}

json TGeoManagerExporter::convertSphere(TGeoSphere *sphere) {
    json j;
    j["type"] = "solid.sphere";
    if (sphere->GetRmin() != 0.0) {
        j["rMin"] = sphere->GetRmin();
    }
    if (sphere->GetRmax() != 0.0) {
        j["rMax"] = sphere->GetRmax();
    }

    return j;
}

json TGeoManagerExporter::convertCone(TGeoCone* cone) {
    json j;
    j["type"] = "solid.cone";
    if (cone->GetDz() != 0.0) {
        j["length"] =  2.0 * cone->GetDz();
    }
    if (cone->GetRmin1() != 0.0) {
        j["rMin1"] = cone->GetRmin1();
    }
    if (cone->GetRmax1() != 0.0) {
        j["rMax1"] = cone->GetRmax1();
    }
    if (cone->GetRmin2() != 0.0) {
        j["rMin2"] = cone->GetRmin2();
    }
    if (cone->GetRmax2() != 0.0) {
        j["rMax2"] = cone->GetRmax2();
    }

    return j;
}

json TGeoManagerExporter::convertBox(TGeoBBox *box) {
    json j;
    j["type"] = "solid.box";
    j["xSize"] = box->GetDX() * 2.0;
    j["ySize"] = box->GetDY() * 2.0;
    j["zSize"] = box->GetDZ() * 2.0;

    return j;
}

json TGeoManagerExporter::convertXtru(TGeoXtru *xtru) {
    json j;
    j["type"] = "solid.extrude";

    std::vector<json> layers;
    double_t* param;
    xtru->SetDimensions(param);
    for (int i = 1; i < (int)param[0] * 4; i += 4) 
        layers.push_back({
            {"x", param[i]},
            {"y", param[i + 1]},
            {"z", param[i + 2]},
            {"scale", param[i + 3]}
        });
    
    //std::cout << layers;
    j["layers"] = layers;

    // std::vector<json> points;
    // double* pnt;
    // xtru->SetPoints(pnt);

    return j;
}

// CONVERT MATRIX

json TGeoManagerExporter::convertMatrix(TGeoMatrix* matrix) {
    json j;
    if (matrix == nullptr) {
        return j;
    }

    if (matrix->IsGeneral()) {
        return convertGeneralMatrix((TGeoGenTrans*) matrix);
    } else if (matrix->IsCombi()) {
        return convertCombination((TGeoCombiTrans*) matrix);   
    } else if (matrix->IsRotation()) {
        return convertRotation((TGeoRotation*) matrix);
    } else if (matrix->IsTranslation() && typeid(*matrix) == typeid(TGeoTranslation)) {
        return convertTranslation((TGeoTranslation*)matrix);
    } else if (matrix->IsScale()) {
        return convertScale((TGeoScale*) matrix);
    } else {
         return j; //make debug case
    }
}

json TGeoManagerExporter::convertRotationBlock(double phi, double theta, double psi) {
    json j, j_deg;

    j_deg["z"] = DegreeToRad(phi);
    j_deg["x"] = DegreeToRad(theta);
    j_deg["y"] = DegreeToRad(psi);
    
    j["rotation"] = j_deg;
    return j;
}

json TGeoManagerExporter::convertTranslationBlock(double x, double y, double z) {
    json j, j_pos;

    j_pos["x"] = x;
    j_pos["y"] = y;
    j_pos["z"] = z;

    j["position"] = j_pos;
    return j;
}

json TGeoManagerExporter::convertScaleBlock(double x, double y, double z) {
    json j, j_sc;

    j_sc["x"] = x;
    j_sc["y"] = y;
    j_sc["z"] = z;

    j["scale"] = j_sc;
    return j;
}

json TGeoManagerExporter::convertGeneralMatrix(TGeoGenTrans* matrix) {
    json j = convertCombination(matrix);
    const double* scale = matrix->GetScale();
    if (scale == nullptr) {
        return j;
    }
    return json_union(j, convertScaleBlock(scale[0], scale[1], scale[2]));
}

json TGeoManagerExporter::convertCombination(TGeoCombiTrans* matrix) {
    TGeoRotation* rotation = matrix->GetRotation();
    json j;
    if (rotation != nullptr) {
        j = convertRotation(rotation);
    }
    
    const double* translation = matrix->GetTranslation();
    if (translation != nullptr) {
        return json_union(j, convertTranslationBlock(translation[0], translation[1], translation[2]));
    } 
    else {
        return j;
    }
}

json TGeoManagerExporter::convertRotation(TGeoRotation* matrix) {
    double phi = 0.0;
    double theta = 0.0;
    double psi = 0.0;
    matrix->GetAngles(phi, theta, psi);
    return convertRotationBlock(phi, theta, psi);    
}

json TGeoManagerExporter::convertTranslation(TGeoTranslation* matrix) {
    const double* translation = matrix->GetTranslation();
    return convertTranslationBlock(translation[0], translation[1], translation[2]);
}

json TGeoManagerExporter::convertScale(TGeoScale* matrix) {
    const double* scale = matrix->GetScale();
    return convertScaleBlock(scale[0], scale[1], scale[2]);
}
