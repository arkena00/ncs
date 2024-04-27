#pragma once

#include <ncs/command/initializer.hpp>

#include <ncs/module/uplugin/commands_impl.hpp>

namespace ncs::modules
{
    class uplugin;
}

// clang-format off
namespace ncs::internal::modules::uplugin
{
    struct uplugin_commands : public ncs::commands<uplugin_commands>
    {
        explicit uplugin_commands(commands_impl& commands_impl_) : ncs::commands<uplugin_commands>("uplugin"), uplugin{ commands_impl_ } {}

        commands_impl& uplugin;
    };

    ncs_root(commands_node, uplugin_commands)
    ncs_command(ncs_module)
    ncs_command_(ncs_module, [this]{ cmd.help(); }, "")

        ncsi::command help{ this, "help", [this]{ cmd.help(); }, "Display uplugin help" };
        ncsi::command version{ this, "version", []{ std::cout << "0.1.0"; }, "Display ncs uplugin version" };

        ncs_command(config)
                ncs_parameter(ueroot, std::string, "", "UE root location")
        ncs_command_(config, [this](auto&& input){ cmd.uplugin.config(input); }, "")

        ncs_command(build)
                ncs_parameter(root, std::string, "", "Project root of plugin")
                ncs_parameter(ue, std::string, "5.4", "Unreal version")
        ncs_command_(build, [this](auto&& input){ cmd.uplugin.build(input); }, "")
    ncs_root_()
}
// clang-format on