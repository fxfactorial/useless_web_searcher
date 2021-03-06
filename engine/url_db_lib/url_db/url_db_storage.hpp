#pragma once

#include "common/db/url_state.hpp"
#include "common/string_view.hpp"
#include "url_db/url_db_offset.hpp"

#include <boost/iostreams/device/mapped_file.hpp>

#include <vector>

namespace usl
{
    namespace common::fs
    {
        class file_loader;
    }

    namespace url_db
    {
        class url_db_storage
        {
        public:
            using data_t = std::vector<uint8_t>;

            explicit url_db_storage(const std::string& working_directory, const common::fs::file_loader& file_loader);

            const data_t &data() const;

            const uint8_t* ptr(offset_t offset) const;
            uint8_t* ptr(offset_t offset);

            offset_t insert(const std::string &url);

            void update_state(offset_t offset, common::db::url_state state);

        private:
            void write_state(boost::iostreams::mapped_file& file, offset_t offset, common::db::url_state state) const;

            void prepare_file_for_insertion(const std::string& url) const;
            void insert_to_file(const std::string &url) const;
            offset_t insert_to_data(const std::string &url);

        private:
            const std::string m_data_file_path;
            data_t m_data;
        };
    }
}
