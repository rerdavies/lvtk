// Autogenerated by generate_lvtk_plugin_info. Do not modify.

#include "lvtk_ui/Lv2PluginInfo.hpp"
#include <memory>

class SamplePluginInfo: public lvtk::ui::Lv2PluginInfo{
public:
    using super=lvtk::ui::Lv2PluginInfo;
    using ptr=std::shared_ptr<SamplePluginInfo>;
    static ptr Create() { return std::make_shared<SamplePluginInfo>(); }

    SamplePluginInfo() {
        using namespace lvtk::ui;

        name("Lvtk Demo Plugin");
        brand("Lvtk");
        label("Test Plugin");
        plugin_class("http://lv2plug.in/ns/lv2core#Plugin");
        supported_features({"http://lv2plug.in/ns/lv2core#hardRTCapable"});
        required_features({});
        optional_features({"http://lv2plug.in/ns/lv2core#hardRTCapable"});
        author_name("Robin Davies");
        author_homepage("https://github.com/sponsors/rerdavies");
        comment("lvtk test plugin");
        extensions({});
        ports({
            Lv2PortInfo_Init {
                .index_ = 0,
                .symbol_ = "level",
                .name_ = "Level",
                .min_value_ = -60,
                .max_value_ = 30,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#InputPort","http://lv2plug.in/ns/lv2core#ControlPort"}},
                .scale_points_ = {Lv2ScalePoint{-60,"-INF"}},
                .is_input_ = true,
                .is_control_port_ = true,
                .units_ = Lv2Units::db,
            },
            Lv2PortInfo_Init {
                .index_ = 1,
                .symbol_ = "vu",
                .name_ = "VU",
                .min_value_ = -60,
                .max_value_ = 20,
                .default_value_ = -60,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#ControlPort","http://lv2plug.in/ns/lv2core#OutputPort"}},
                .is_output_ = true,
                .is_control_port_ = true,
                .units_ = Lv2Units::db,
            },
            Lv2PortInfo_Init {
                .index_ = 2,
                .symbol_ = "lfoRate",
                .name_ = "Rate",
                .min_value_ = 0.100000001490116,
                .max_value_ = 3,
                .default_value_ = 3,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#ControlPort","http://lv2plug.in/ns/lv2core#InputPort"}},
                .is_input_ = true,
                .is_control_port_ = true,
                .is_logarithmic_ = true,
                .units_ = Lv2Units::hz,
            },
            Lv2PortInfo_Init {
                .index_ = 3,
                .symbol_ = "lfoDepth",
                .name_ = "Depth",
                .default_value_ = 1,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#ControlPort","http://lv2plug.in/ns/lv2core#InputPort"}},
                .is_input_ = true,
                .is_control_port_ = true,
            },
            Lv2PortInfo_Init {
                .index_ = 4,
                .symbol_ = "lfoOut",
                .name_ = "LFO",
                .min_value_ = -1,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#ControlPort","http://lv2plug.in/ns/lv2core#OutputPort"}},
                .is_output_ = true,
                .is_control_port_ = true,
            },
            Lv2PortInfo_Init {
                .index_ = 5,
                .symbol_ = "vuOutL",
                .name_ = "L",
                .min_value_ = -60,
                .max_value_ = 12,
                .default_value_ = -60,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#ControlPort","http://lv2plug.in/ns/lv2core#OutputPort"}},
                .scale_points_ = {Lv2ScalePoint{-60,"-INF"}},
                .is_output_ = true,
                .is_control_port_ = true,
                .port_group_ = "https://rerdavies.github.io/lvtk/test_plugin#vuOutGroup",
                .designation_ = "http://lv2plug.in/ns/ext/port-groups#left",
                .units_ = Lv2Units::db,
            },
            Lv2PortInfo_Init {
                .index_ = 6,
                .symbol_ = "vuOutR",
                .name_ = "R",
                .min_value_ = -60,
                .max_value_ = 12,
                .default_value_ = -60,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#ControlPort","http://lv2plug.in/ns/lv2core#OutputPort"}},
                .scale_points_ = {Lv2ScalePoint{-60,"-INF"}},
                .is_output_ = true,
                .is_control_port_ = true,
                .port_group_ = "https://rerdavies.github.io/lvtk/test_plugin#vuOutGroup",
                .designation_ = "http://lv2plug.in/ns/ext/port-groups#right",
                .units_ = Lv2Units::db,
            },
            Lv2PortInfo_Init {
                .index_ = 7,
                .symbol_ = "bass",
                .name_ = "Bass",
                .default_value_ = 0.5,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#ControlPort","http://lv2plug.in/ns/lv2core#InputPort"}},
                .is_input_ = true,
                .is_control_port_ = true,
                .port_group_ = "https://rerdavies.github.io/lvtk/test_plugin#eqGroup",
            },
            Lv2PortInfo_Init {
                .index_ = 8,
                .symbol_ = "mid",
                .name_ = "Mid",
                .default_value_ = 0.5,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#InputPort","http://lv2plug.in/ns/lv2core#ControlPort"}},
                .is_input_ = true,
                .is_control_port_ = true,
                .port_group_ = "https://rerdavies.github.io/lvtk/test_plugin#eqGroup",
            },
            Lv2PortInfo_Init {
                .index_ = 9,
                .symbol_ = "treble",
                .name_ = "Treb",
                .default_value_ = 0.5,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#InputPort","http://lv2plug.in/ns/lv2core#ControlPort"}},
                .is_input_ = true,
                .is_control_port_ = true,
                .port_group_ = "https://rerdavies.github.io/lvtk/test_plugin#eqGroup",
            },
            Lv2PortInfo_Init {
                .index_ = 10,
                .symbol_ = "toneStack",
                .name_ = "ToneStack",
                .max_value_ = 2,
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#ControlPort","http://lv2plug.in/ns/lv2core#InputPort"}},
                .scale_points_ = {Lv2ScalePoint{0,"Bassman"},Lv2ScalePoint{1,"JCM8000"},Lv2ScalePoint{2,"Baxandall"}},
                .is_input_ = true,
                .is_control_port_ = true,
                .enumeration_property_ = true,
                .port_group_ = "https://rerdavies.github.io/lvtk/test_plugin#eqGroup",
            },
            Lv2PortInfo_Init {
                .index_ = 11,
                .symbol_ = "in",
                .name_ = "In",
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#AudioPort","http://lv2plug.in/ns/lv2core#InputPort"}},
                .is_input_ = true,
                .is_audio_port_ = true,
            },
            Lv2PortInfo_Init {
                .index_ = 12,
                .symbol_ = "out",
                .name_ = "Out",
                .classes_ = Lv2PluginClasses{{"http://lv2plug.in/ns/lv2core#AudioPort","http://lv2plug.in/ns/lv2core#OutputPort"}},
                .is_output_ = true,
                .is_audio_port_ = true,
            },
        });
        port_groups({Lv2PortGroup{"https://rerdavies.github.io/lvtk/test_plugin#vuOutGroup","vuOutGroup","Out"},Lv2PortGroup{"https://rerdavies.github.io/lvtk/test_plugin#eqGroup","eqGroup","Tone"}});
        has_factory_presets(false);
        piPedalUI(PiPedalUI{
            {
            },
            {
            },
        });
    }
};