#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include "Exporter.h"

TEST_CASE( "Shapes" ) {
    TGeoManagerExporter exp;
    TGeoManager *geom = new TGeoManager("simple1", "Simple geometry");
    TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);

    SECTION( "Box" ) {
        TGeoBBox *box = new TGeoBBox("BOX", 20, 30, 40);
        json j_exporter_box = exp.convertBox(box);

        json j_box = {
            {"type", "solid.box"},
            {"xSize", 40},
            {"ySize", 60},
            {"zSize", 80}
        };

        REQUIRE( j_exporter_box == j_box );
    } 

    SECTION( "Cone" ) {
        TGeoCone *cone = new TGeoCone("Cone", 40, 10, 20, 35, 45);
        json j_exporter_cone = exp.convertCone(cone);

        json j_cone = {
            {"type", "solid.cone"},
            {"length", 80},
            {"rMin1", 10},
            {"rMax1", 20},
            {"rMin2", 35},
            {"rMax2", 45}
            
        };

        REQUIRE( j_exporter_cone == j_cone );
    } 

}


// TEST_CASE( "big test" ) {

// }

// TEST_CASE( "matrix" ) {

// }