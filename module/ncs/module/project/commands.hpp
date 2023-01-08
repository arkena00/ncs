#ifndef MODULE_NCS_MODULE_PROJECT_COMMANDS_HPP_NCS
#define MODULE_NCS_MODULE_PROJECT_COMMANDS_HPP_NCS

#include <ncs/command/initializer.hpp>

#include <ncs/module/project/commands_impl.hpp>

namespace ncs::modules
{
    class project;
}

// clang-format off
namespace ncs::internal::modules::project
{
    // ncs_module(project) #define ..
    struct project_commands : public ncs::commands<project_commands>
    {
        explicit project_commands(commands_impl& commands_impl_) : ncs::commands<project_commands>("project"), project{ commands_impl_ } {}

        commands_impl& project;
    };

    ncs_root(commands_node, project_commands)
    ncs_command(ncs_module)
    ncs_command_(ncs_module, [this]{ cmd.help(); }, "")

        ncsi::command help{ this, "help", [this]{ cmd.help(); }, "Display project  help" };
        ncsi::command version{ this, "version", []{ std::cout << "0.2.1"; }, "Display ncs project version" };

        ncs_command(make)
                ncs_required(tpl, std::string, "Location to the project template")
                ncs_parameter(name, std::string, "project", "Project name")
                ncs_parameter(license, std::string, "mit", "License")
        ncs_command_(make, [this](auto&& input){ cmd.project.make(input); }, "")
    ncs_root_()
}
// clang-format on

#endif // MODULE_NCS_MODULE_PROJECT_COMMANDS_HPP_NCS