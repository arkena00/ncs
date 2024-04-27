#pragma once

namespace ncs { class parameters; }
namespace ncs::modules { class uplugin; }

namespace ncs::internal::modules::uplugin
{
    struct commands_impl
    {
        explicit commands_impl(ncs::modules::uplugin& uplugin) : uplugin_{ uplugin } {}

        void config(const ncs::parameters& input);
        void build(const ncs::parameters& input);

        void run(const std::string& command, const std::filesystem::path& execution_path);

        std::string get_plugin_version() const;

    private:
        bool build_success_ = false;
        std::string plugin_name_;
        std::string uplugin_path_;
        std::string output_dir_;
        ncs::modules::uplugin& uplugin_;
    };

} // ncs