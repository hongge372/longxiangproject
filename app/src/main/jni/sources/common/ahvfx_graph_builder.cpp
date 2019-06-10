//
//  ahvfx_graph_builder.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/21.
//  Copyright © 2019 AH. All rights reserved.
//

#include <stdlib.h>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <sstream>
#include "ahvfx_graph_builder.hpp"
#include "ahvfx_request_node.hpp"
#include "ahvfx_file_manager.hpp"
#include "ahvfx_holder_node.hpp"
#include "ahvfx_util.hpp"
#include "ahvfx_gl_library.hpp"
#include "ahvfx_uniform_data.hpp"

extern "C" {
    char *readbuf(const char *, long &);
}

static std::string graph_vfx = "/graph.vfx";

typedef neb::CJsonObject dictionary;
typedef neb::CJsonObject array;

using namespace ahvfx;

static std::shared_ptr<ahvfx::graph_node> get_graph_node(std::shared_ptr<ahvfx::graph> graph, dictionary &dict) {
    std::shared_ptr<ahvfx::graph_node> rs = NULL;
    std::string str;
    if (dict.Get("type", str)) {
        // graph node
        if (str == "render_node" || str == "graph_node") {
            rs = std::shared_ptr<ahvfx::graph_node>(new ahvfx::graph_node());
            bool whether = false;
            if (dict.Get("initial", whether)) {
                rs->set_initial(whether);
            }
            if (dict.Get("terminal", whether)) {
                rs->set_terminal(whether);
            }
            array ranges;
            if (dict.Get("update", whether) || dict.Get("always_update", whether)) {
                rs->always_update = true;
            } else if (dict.Get("ranges", ranges)) {
                if (ranges.IsArray()) {
                    for (int i = 0; i < ranges.GetArraySize(); ++i) {
                        dictionary range = ranges[i];
                        uint32_t v1 = 0, v2 = 0, v3 = 0;
                        range.Get("instance_render", v1);
                        range.Get("start_index", v2);
                        range.Get("end_index", v3);
                        rs->ranges.push_back(ahvfx::make_range(v1, v2, v3));
                    }
                }
            }
            if (dict.Get("name", str)) {
                rs->set_name(str);
            }
            if (dict.Get("render_style", str)) {
                if (str == "merge") {
                    rs->set_render_style(ahvfx::graph_node_render_style::merge);
                } else if (str == "coverage") {
                    rs->set_render_style(ahvfx::graph_node_render_style::coverage);
                }
            }
            if (dict.Get("program", str)) {
                rs->program = graph->restore_program(str);
            }
            
            array unidarr;
            if (dict.Get("uniform_datas", unidarr)) {
                if (unidarr.IsArray()) {
                    for (int i = 0; i < unidarr.GetArraySize(); ++i) {
                        std::string unid_name;
                        if (unidarr.Get(i, unid_name)) {
                            rs->uniform_datas.push_back(graph->restore_unid(unid_name));
                        }
                    }
                }
            }
            
            array cc;
            if (dict.Get("clear_color", cc)) {
                if (cc.IsArray()) {
                    float v[4];
                    for (int i = 0; i < cc.GetArraySize(); ++i) {
                        cc.Get(i, v[i]);
                    }
                    rs->clearColor = ahvfx::vec4<float>(v[0], v[1], v[2], v[3]);
                }
            }
        }
        
        // requset node
        else if (str == "image_node" || str == "video_node") {
            std::string type = str;
            std::shared_ptr<ahvfx::request_node> req = std::shared_ptr<ahvfx::request_node>(new ahvfx::request_node());
            rs = req;
            bool whether = false;
            if (dict.Get("initial", whether)) {
                rs->set_initial(whether);
            }
            if (dict.Get("terminal", whether)) {
                rs->set_terminal(whether);
            }
            array ranges;
            if (dict.Get("update", whether) || dict.Get("always_update", whether)) {
                rs->always_update = true;
            } else if (dict.Get("ranges", ranges)) {
                if (ranges.IsArray()) {
                    for (int i = 0; i < ranges.GetArraySize(); ++i) {
                        dictionary range = ranges[i];
                        uint32_t v1 = 0, v2 = 0, v3 = 0;
                        range.Get("instance_render", v1);
                        range.Get("start_index", v2);
                        range.Get("end_index", v3);
                        rs->ranges.push_back(ahvfx::make_range(v1, v2, v3));
                    }
                }
            }
            if (dict.Get("name", str)) {
                req->set_name(graph->get_name(), str);
            }
            if (dict.Get("program", str)) {
                rs->program = graph->restore_program(str);
            }
            
            int interval = 1;
            if (dict.Get("interval", interval)) {
                req->set_animation_interval(interval);
            }
            
            array asset_paths;
            std::string format_name;
            if (dict.Get("asset_paths", asset_paths)) {
                if (asset_paths.IsArray()) {
                    int array_size = asset_paths.GetArraySize();
                    for (int i = 0; i < array_size; ++i) {
                        std::string asset_path, asset_root_path;
                        dictionary d = asset_paths[i];
                        if (d.Get("format_name", format_name) && d.Get("root_path", asset_root_path)) {
                            uint32_t start = 0, end = 0;
                            asset_paths[i].Get("start", start);
                            asset_paths[i].Get("end", end);
                            for (int j = start; j <= end; ++j) {
                                char format_path[1024];
                                sprintf(format_path, format_name.c_str(), j);
                                req->add_referred_asset(asset_root_path + "/" + format_path);
                            }
                        } else if (asset_paths[i].Get("path", asset_path)) {
                            req->add_referred_asset(asset_path);                            
                        }
                    }
                }
            }
            uint32_t node_type = 0;
            if (type == "image_node") {
                node_type = 0;
            } else if (type == "video_node") {
                node_type = 1;
            }
            req->set_request_node_type(node_type);
            req->set_callbacks(ahvfx::graph_builder::default_builder()->callback);
            req->request_init();
        }
        
        // lagger node
        else if (str == "lagger_node" || str == "holder_node") {
            
            int capacity = 0;
            if (dict.Get("capacity", capacity) && capacity > 0) {
                std::shared_ptr<ahvfx::lagger_node> req = std::shared_ptr<ahvfx::lagger_node>(new ahvfx::lagger_node(capacity));
                rs = req;
                bool whether = false;
                if (dict.Get("initial", whether)) {
                    rs->set_initial(whether);
                }
                if (dict.Get("terminal", whether)) {
                    rs->set_terminal(whether);
                }
                array ranges;
                if (dict.Get("update", whether) || dict.Get("always_update", whether)) {
                    rs->always_update = true;
                } else if (dict.Get("ranges", ranges)) {
                    if (ranges.IsArray()) {
                        for (int i = 0; i < ranges.GetArraySize(); ++i) {
                            dictionary range = ranges[i];
                            uint32_t v1 = 0, v2 = 0, v3 = 0;
                            range.Get("instance_render", v1);
                            range.Get("start_index", v2);
                            range.Get("end_index", v3);
                            rs->ranges.push_back(ahvfx::make_range(v1, v2, v3));
                        }
                    }
                }
                if (dict.Get("name", str)) {
                    rs->set_name(str);
                }
                int output_num = 0;
                if (dict.Get("output_num", output_num)) {
                    req->set_output_num(output_num);
                }
            }
        }

    } else {
        ahvfx_log("node has no type?");
        assert(0);
    }
    return rs;
}

static inline void split(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
        
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

static inline std::string read_string(std::string file) {
    
    long bufsz;
    char *cs = readbuf(file.c_str(), bufsz);
    if (!bufsz || !cs) {
        return "";
    }
    std::vector<std::string> lines;
    split(std::string(cs), lines, "\n");
    free(cs);
    std::string rs;
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        rs += ahvfx::gl_library::library_inline(std::ref(*it));
        rs += "\n";
    }
    return rs;
}

static std::shared_ptr<ahvfx::program> get_program(dictionary &dict) {
    std::shared_ptr<ahvfx::program> rs = NULL;
    std::string str;
    if (dict.Get("name", str)) {
        std::string vsh, fsh;
        if (!dict.Get("vsh_path", vsh)) {
            ahvfx_log("no vert funcs.");
        }
        if (!dict.Get("fsh_path", fsh)) {
            ahvfx_log("no frag funcs.");
        }
        std::string vs = read_string(vsh);
        std::string fs = read_string(fsh);
        rs = std::shared_ptr<ahvfx::program>(new ahvfx::program(vs.empty() ? NULL : vs.c_str(), fs.empty() ? NULL : fs.c_str()));
        rs->set_name(str);
    }
    return rs;
}

static ahvfx::uniform_data get_uniform_data(dictionary &dict) {
    std::string name;
    if (dict.Get("name", name)) {
        array data_array;
        if (dict.Get("data", data_array)) {
            if (data_array.IsArray()) {
                int data_type = 0;
                dict.Get("type", data_type);
                ahvfx::uniform_data data((ahvfx::uniform_data::type)data_type, (uint32_t)data_array.GetArraySize());
                data.data_name = name;
                for (int i = 0; i < data_array.GetArraySize(); ++i) {
                    float value;
                    if (data_array.Get(i, value)) {
                        data.pack(i, &value);
                    }
                }
#ifdef DEBUG
                data.print();
#endif
                return data;
            }
        }
    }
    return ahvfx::uniform_data();
}

// links
static void link_nodes(std::shared_ptr<ahvfx::graph> graph, std::string src, std::string dst, uint32_t index) {
    ahvfx::graph_node_ptr src_node = graph->restore(src);
    ahvfx::graph_node_ptr dst_node = graph->restore(dst);
    if (src_node && dst_node) {
        src_node->link(dst_node, index);
    }
}

std::shared_ptr<ahvfx::graph> graph_builder::build_graph(std::string root, std::string pack) {
    std::shared_ptr<ahvfx::graph> graph = NULL;
    std::string base = root + "/" + pack;
    std::string full = base + graph_vfx;
    if (access(full.c_str(), 0) != -1) {
        long readsz;
        dictionary Json(readbuf(full.c_str(), readsz));
#if DEBUG1
        std::cout << Json.ToFormattedString() << std::endl;
#endif
        file_manager::default_manager()->add_root_path(root, pack, Json);
#if DEBUG1
        std::cout << Json.ToFormattedString() << std::endl;
#endif
        if (Json.IsEmpty() == false) {
            
            graph = std::shared_ptr<ahvfx::graph>(new ahvfx::graph());
            graph->set_name(pack);
            
            // version
            uint32_t version = 0;
            if (Json.Get("version", version)) {
                graph->set_build_version(version);
            } else {
                graph->set_build_version(1000);
            }
            
            // unifroms
            array uniform_data_array;
            if (Json.Get("uniform_datas", uniform_data_array)) {
                if (uniform_data_array.IsArray()) {
                    for (int i = 0; i < uniform_data_array.GetArraySize(); ++i) {
                        ahvfx::uniform_data unid = get_uniform_data(uniform_data_array[i]);
                        graph->store(unid);
                    }
                }
            }
    
            // programs
            dictionary programs;
            if (Json.Get("programs", programs)) {
                if (programs.IsArray()) {
                    int sz = programs.GetArraySize();
                    for (int i = 0; i < sz; ++i) {
                        dictionary dict = programs[i];
                        std::shared_ptr<ahvfx::program> rs = get_program(dict);
                        if (rs) {
                            graph->store(rs);
                        }
                    }
                }
            } else {
                ahvfx_log("%s", programs.GetErrMsg().c_str());
            }
            
            // nodes
            dictionary nodes;
            if (Json.Get("nodes", nodes)) {
                if (nodes.IsArray()) {
                    int sz = nodes.GetArraySize();
                    for (int i = 0; i < sz; ++i) {
                        dictionary dict = nodes[i];
                        std::shared_ptr<ahvfx::graph_node> rs = get_graph_node(graph, dict);
                        if (rs) {
                            graph->store(rs);
                        }
                    }
                }
            } else {
                ahvfx_log("%s", nodes.GetErrMsg().c_str());
            }
            
            // links
            dictionary links;
            if (Json.Get("links", links)) {
                if (links.IsArray()) {
                    int sz = links.GetArraySize();
                    for (int i = 0; i < sz; ++i) {
                        dictionary dict = links[i];
                        std::string src, dst;
                        if (dict.Get("src", src) && dict.Get("dst", dst)) {
                            uint32_t index = 0;
                            dict.Get("index", index);
                            link_nodes(graph, src, dst, index);
                        }
                    }
                }
            } else {
                ahvfx_log("%s", links.GetErrMsg().c_str());
            }
        }
    }
    
    return graph;
}


graph_builder * graph_builder::default_builder() {
    static graph_builder *builder = NULL;
#if 0
    static std::once_flag oc;
    std::call_once(oc, [&]{
        builder = new graph_builder();
    });
#endif
    return builder;
}
