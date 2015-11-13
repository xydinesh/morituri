/*
 * navteq_plugin.cpp
 *
 *  Created on: 11.06.2015
 *      Author: philip
 */

#include <osmium/io/any_input.hpp>
#include <osmium/io/any_output.hpp>

#include <gdal/ogr_api.h>
#include <boost/filesystem/operations.hpp>

#include "navteq_plugin.hpp"
#include "navteq.hpp"
#include "../util.hpp"

/*

 Convert navteq data into routable OSM files.

 */

navteq_plugin::navteq_plugin() {
    name = "Navteq Plugin";
}

navteq_plugin::~navteq_plugin() {
}

bool navteq_plugin::is_valid_format(std::string filename) {
    if (filename.length() > 3) filename = filename.substr(filename.length() - 3);
    for (auto i : filename)
        i = std::tolower(i);
    if (filename == "pbf") return true;
    if (filename == "osm") return true;
    return false;
}

bool navteq_plugin::check_input(const char* input_path, const char* output_file) {
    if (!boost::filesystem::is_directory(input_path))
        throw(std::runtime_error("directory " + std::string(input_path) + " does not exist"));
    if (output_file) {
        std::string output_path = boost::filesystem::path(output_file).parent_path().string();
        if (!boost::filesystem::is_directory(output_path))
            throw(std::runtime_error("output directory " + output_path + " does not exist"));
        if (!is_valid_format(std::string(output_file)))
            throw(format_error("unknown format for outputfile: " + std::string(output_file)));
    }

    if (!shp_file_exists(input_path + STREETS_SHP)) return false;
    if (!shp_file_exists(input_path + ADMINBNDY_1_SHP)) std::cerr << "administrative boundaries level 1 are missing\n";
    if (!shp_file_exists(input_path + ADMINBNDY_2_SHP)) std::cerr << "administrative boundaries level 2 are missing\n";
    if (!shp_file_exists(input_path + ADMINBNDY_3_SHP)) std::cerr << "administrative boundaries level 3 are missing\n";
    if (!shp_file_exists(input_path + ADMINBNDY_4_SHP)) std::cerr << "administrative boundaries level 4 are missing\n";
    if (!shp_file_exists(input_path + ADMINBNDY_5_SHP)) std::cerr << "administrative boundaries level 5 are missing\n";

    if (!dbf_file_exists(input_path + MTD_AREA_DBF)) return false;
    if (!dbf_file_exists(input_path + RDMS_DBF)) return false;
    if (!dbf_file_exists(input_path + CDMS_DBF)) return false;
    if (!dbf_file_exists(input_path + ZLEVELS_DBF)) return false;

    this->plugin_setup(input_path, output_file);
    return true;
}

void navteq_plugin::execute() {

    add_street_shape_to_osmium(read_shape_file(input_path + STREETS_SHP), input_path);
    assert__id_uniqueness();

    process_turn_restrictions(read_dbf_file(input_path + RDMS_DBF), read_dbf_file(input_path + CDMS_DBF));
    assert__id_uniqueness();

//    assert__node_locations_uniqueness();

// todo admin-levels only apply to the US => more generic for all countries
    if (shp_file_exists(input_path + ADMINBNDY_1_SHP))
        add_admin_shape_to_osmium(read_shape_file(input_path + ADMINBNDY_1_SHP), input_path);
    if (shp_file_exists(input_path + ADMINBNDY_2_SHP))
        add_admin_shape_to_osmium(read_shape_file(input_path + ADMINBNDY_2_SHP), input_path);
    if (shp_file_exists(input_path + ADMINBNDY_3_SHP))
        add_admin_shape_to_osmium(read_shape_file(input_path + ADMINBNDY_3_SHP), input_path);
    if (shp_file_exists(input_path + ADMINBNDY_4_SHP))
        add_admin_shape_to_osmium(read_shape_file(input_path + ADMINBNDY_4_SHP), input_path);
    if (shp_file_exists(input_path + ADMINBNDY_5_SHP))
        add_admin_shape_to_osmium(read_shape_file(input_path + ADMINBNDY_5_SHP), input_path);
    g_mtd_area_map.clear();

    std::string output = output_path;
    if (!output.empty()) {
        std::cout << "writing... " << output << std::endl;

        osmium::io::File outfile(output);
        osmium::io::Header hdr;
        hdr.set("generator", "osmium");
        hdr.set("xml_josm_upload", "false");

        osmium::io::Writer writer(outfile, hdr, osmium::io::overwrite::allow);
        writer(std::move(g_node_buffer));
        writer(std::move(g_way_buffer));
        writer(std::move(g_rel_buffer));
        writer.close();
    }

    std::cout << std::endl << "fin" << std::endl;
}
