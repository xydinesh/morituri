/*
 * navteq_types.hpp
 *
 *  Created on: 04.11.2015
 *      Author: philip
 */

#ifndef PLUGINS_NAVTEQ_NAVTEQ_TYPES_HPP_
#define PLUGINS_NAVTEQ_NAVTEQ_TYPES_HPP_

#include <unordered_map>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/types.hpp>
#include <boost/filesystem/path.hpp>
#include <gdal/ogrsf_frmts.h>

typedef std::vector<boost::filesystem::path> path_vector_type;

typedef uint64_t cond_id_type;
typedef uint64_t mod_typ_type;
typedef uint64_t mod_val_type;

typedef std::pair<mod_typ_type, mod_val_type> mod_pair_type;

struct cntry_ref_type {
    char unit_measure;
    char speed_limit_unit[3];
    char iso_code[3];
    cntry_ref_type(){}
    cntry_ref_type(char unit_measure, const char* speed_limit_unit, const char* iso_code) {
        this->unit_measure = unit_measure;
        strcpy(this->speed_limit_unit, speed_limit_unit);
        strcpy(this->iso_code, iso_code);
    }
    bool operator==(cntry_ref_type rhs){
        if (this->unit_measure != rhs.unit_measure) return false;
        if (strcmp(this->speed_limit_unit, rhs.speed_limit_unit)) return false;
        if (strcmp(this->iso_code, rhs.iso_code)) return false;
        return true;
    }
    bool operator!=(cntry_ref_type rhs){
        return !(*this == rhs);
    }

};

struct mod_group_type {
    const char* lang_code;
    mod_typ_type mod_type;
    mod_val_type mod_val;
    mod_group_type(mod_typ_type mod_type, mod_val_type mod_val, const char* lang_code = nullptr){
        this->lang_code = lang_code;
        this->mod_type = mod_type;
        this->mod_val = mod_val;
    }
};

typedef uint64_t area_id_type;
typedef uint64_t govt_code_type;
typedef std::map<area_id_type, govt_code_type> area_id_govt_code_map_type;
typedef std::map<govt_code_type, cntry_ref_type> cntry_ref_map_type;

typedef std::unordered_map<cond_id_type, mod_group_type> cnd_mod_map_type;

typedef uint64_t link_id_type;
typedef std::multimap<link_id_type, cond_id_type> cdms_map_type;

// vector of osm_ids
typedef std::vector<osmium::unsigned_object_id_type> osm_id_vector_type;

// maps location to node ids
typedef std::map<osmium::Location, osmium::unsigned_object_id_type> node_map_type;

// pair of [Location, osm_id]
typedef std::pair<osmium::Location, osmium::unsigned_object_id_type> loc_osmid_pair_type;
// vector of pairs of [Location, osm_id]
typedef std::vector<loc_osmid_pair_type> node_vector_type;

// maps link ids to a vector of osm_ids
typedef std::map<link_id_type, osm_id_vector_type> link_id_map_type;

typedef std::vector<link_id_type> link_id_vector_type;

/* z-level types */
// type of z-levels (range -4 to +5)
typedef short z_lvl_type;

typedef std::pair<ushort, z_lvl_type> index_z_lvl_pair_type;
typedef std::vector<index_z_lvl_pair_type> index_z_lvl_vector_type;
// maps navteq link_ids to pairs of <index, z_lvl>
typedef std::map<link_id_type, index_z_lvl_vector_type> z_lvl_map;

// pair [Location, z_level] identifies nodes precisely
typedef std::pair<osmium::Location, z_lvl_type> node_id_type;
// maps pair [Location, z_level] to osm_id.
typedef std::map<node_id_type, osmium::unsigned_object_id_type> z_lvl_nodes_map_type;


typedef osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, size_t> osm_id_to_offset_map_type;

typedef std::unique_ptr<OGRFeature> ogr_feature_uptr;
typedef std::unique_ptr<OGRLayer> ogr_layer_uptr;
typedef std::vector<ogr_layer_uptr> ogr_layer_uptr_vector;

#endif /* PLUGINS_NAVTEQ_NAVTEQ_TYPES_HPP_ */
