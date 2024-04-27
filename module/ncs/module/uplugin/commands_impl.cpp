#include <ncs/core.hpp>
#include <filesystem>
#include <fstream>

#include <ncs/module/uplugin/commands_impl.hpp>
#include <ncs/module/uplugin/uplugin.hpp>
#include "tiny-process-library/process.hpp"

namespace ncs::internal::modules::uplugin
{
    namespace fs = std::filesystem;

    void commands_impl::config(const ncs::parameters& input)
    {

    }

    void commands_impl::build(const ncs::parameters& input)
    {
        try {
        std::string ue_version = input.value("ue");
        uplugin_.core().log("UE version: {}", ue_version);
        std::string uat_command = "C:/UE_" + ue_version + "/Engine/Build/BatchFiles/RunUAT.bat";

        std::string plugin_root = input.value("root");
        if (plugin_root.empty()) plugin_root = fs::current_path().generic_string();
        plugin_root += "/";
        uplugin_.core().log("Plugin root: {}", plugin_root);
        output_dir_ = plugin_root + "Output/";
        std::string deploy_root = plugin_root + "Deploy/";

        fs::path plugin_root_path { plugin_root };
        if (!fs::is_directory(plugin_root_path / "Plugins")) return uplugin_.core().log("Error: plugin path not found: {}", plugin_root);
        plugin_root_path = plugin_root_path / "Plugins";

        plugin_name_ = "";
        for (const auto& source_entry : fs::recursive_directory_iterator(plugin_root_path))
        {
            if (fs::is_directory(source_entry)) {
                plugin_name_ = source_entry.path().filename().generic_string();
                break;
            }
        }

        uplugin_path_ = (plugin_root_path / plugin_name_ / (plugin_name_ + ".uplugin")).generic_string();
        std::string plugin_version = get_plugin_version();

        std::string build_command = uat_command + " BuildPlugin -Plugin=" + uplugin_path_ + " -Package=" + plugin_root + "/Output" + " -Rocket";
        uplugin_.core().log("Building {} version {} ...", plugin_name_, plugin_version);
        run(build_command, plugin_root_path / plugin_name_);

        if (build_success_ || 1)
        {
            uplugin_.core().log("Archiving sources ...");
            fs::remove_all(fs::path{ output_dir_ } / "Binaries");
            fs::remove_all(fs::path{ output_dir_ } / "Intermediate");
            std::string archive_name = plugin_name_ + "-" + plugin_version + "-UE" + ue_version + ".zip";
            std::string zip_command = uat_command + " ZipUtils -archive=" + (deploy_root + archive_name) + " -add=" + output_dir_;
            run(zip_command, plugin_root_path / plugin_name_);
            fs::remove_all(fs::path{ output_dir_ });
        }

        uplugin_.core().log("Building documentation ...");
        fs::path doc_path = (plugin_root_path / plugin_name_ / "Deploy/Website/mkdocs.yml");
        if (!fs::exists(doc_path)) return uplugin_.core().log("Doc not found at {}", doc_path.generic_string());
        std::string mkdoc_command = "mkdocs build -q -f " + doc_path.generic_string() + " -d " + deploy_root + "Documentation";
        system(mkdoc_command.c_str());

        } catch(const std::exception& e) { std::cout << e.what(); }
    }

    void commands_impl::run(const std::string& command, const fs::path& execution_path)
    {
        std::vector<std::string> process_args;
        for (auto line : std::views::split(command, " "))
        {
            process_args.emplace_back(line.begin(), line.end());
        }

        for (const auto& arg : process_args)
        TinyProcessLib::Process process(
            process_args,
            execution_path.generic_string(),
            [&command, this](const char* bytes, size_t n) {
                std::string output{ bytes, n };

                auto beginpos = output.find("UnrealBuildTool.dll\" UnrealGame");
                if (beginpos != std::string::npos)
                {
                    auto endpos = output.find("-Project", beginpos);
                    std::string target = output.substr(beginpos + 32, endpos - beginpos - 32);
                    uplugin_.core().log("Target platform {}", target);
                }
                if (output.contains(": error"))
                {
                    auto plugin_name_pos = output.find("Source");
                    std::string error_message = output.substr(plugin_name_pos + 8 + plugin_name_.size());
                    std::cout << hue::red << error_message << hue::reset;
                }
                //if (output.contains("Compile") || output.contains("Link")) std::cout << hue::grey << output << hue::reset;
                if (output.contains("BUILD FAILED")) std::cout << hue::light_blue << "[ncs] " << hue::red << "Build failed" << hue::reset << std::endl;
                if (output.contains("BUILD SUCCESSFUL"))
                {
                    std::cout << hue::light_blue << "[ncs] " << hue::green << "Success !" << hue::reset << std::endl;
                    build_success_ = true;
                }
            },
            [&command, this](const char* bytes, size_t n) {
                //std::cout << "\n" << std::string{ bytes, n } << std::endl;
            });
    }

    std::string commands_impl::get_plugin_version() const
    {
        std::string version_name = "1.0";
        std::ifstream ifs{ uplugin_path_, std::ios::in };

        if (!ifs.is_open()) uplugin_.core().log("Error opening file {}", uplugin_path_);
        else
        {
            std::string file_content{ std::istreambuf_iterator<char>(ifs), {} };
            auto version_pos = file_content.find("VersionName");
            if (version_pos != std::string::npos)
            {
                auto version_endpos = file_content.find(",", version_pos);
                version_name = file_content.substr(version_pos + 15, version_endpos - version_pos - 16);
            }
        }
        return version_name;
    }
}