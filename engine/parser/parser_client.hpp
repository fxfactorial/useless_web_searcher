#pragma once

#include <string>

namespace usl::parser
{
    namespace content
    {
        class content_parser;
    }

    class file_paths_to_parse_provider;

    class parser_client
    {
    public:
        parser_client(file_paths_to_parse_provider& file_paths_provider, content::content_parser& parser);

        void run();

    private:
        struct url_and_site
        {
            std::string url;
            std::string site_content;
        };

        url_and_site extract_url_and_site(const std::string& file_content);

        file_paths_to_parse_provider& m_file_paths_provider;
        content::content_parser& m_parser;
    };
}
