#include "CLI11/CLI11.hpp"

int main(int argc, char **argv)
{
    CLI::App app{"CLI Tool"};

    std::string lang = "en";
    app.add_option("-l,--lang", lang, "Language code")->default_val("en");

    CLI11_PARSE(app, argc, argv);

    std::cout << "hi" << std::endl;

    getchar();

    return 0;
}
