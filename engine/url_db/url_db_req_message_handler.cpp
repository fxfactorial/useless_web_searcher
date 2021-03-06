#include "url_db_req_message_handler.hpp"

#include "common/db/url_db_request_keys.hpp"
#include "common/db/url_id.hpp"

#include "url_db/url_db.hpp"
#include "url_db/db_entry_to_process.hpp"

#include <boost/property_tree/json_parser.hpp>

#include <easylogging/easylogging++.h>

#include <iterator>
#include <sstream>

namespace usl::url_db
{
    url_db_req_message_handler::url_db_req_message_handler(url_db& db)
        : m_db{ db }
    {}

    zmq::message_t url_db_req_message_handler::handle(const zmq::message_t& req)
    {
        const auto str_req = std::string{ req.data<const char>(), req.size() };
        std::istringstream iss{ str_req };

        boost::property_tree::ptree tree;
        boost::property_tree::read_json(iss, tree);

        const auto msg_type = tree.get<std::string>("type");
        if(msg_type == common::db::request_keys::k_insert)
        {
            return insert(tree);
        }
        if(msg_type == common::db::request_keys::k_get_for_processing)
        {
            return get_for_processing(tree);
        }
        if(msg_type == common::db::request_keys::k_update_state_as_processed)
        {
            return update_state_as_processed(tree);
        }
        if(msg_type == common::db::request_keys::k_get)
        {
            return get(tree);
        }

        return zmq::message_t();
    }

    zmq::message_t url_db_req_message_handler::get_for_processing(boost::property_tree::ptree& parsed_req)
    {
        auto to_process = m_db.get_for_processing();
        if(!to_process)
        {
            return zmq::message_t();
        }

        LOG(INFO) << "url_db_req_message_handler get_for_processing sending: " << to_process->id <<", " << to_process->url;

        boost::property_tree::ptree tree;
        tree.put("url", to_process->url);
        tree.put("id", to_process->id);

        std::ostringstream oss;
        boost::property_tree::write_json(oss, tree);
        const auto str_resp = oss.str();

        return zmq::message_t{ std::cbegin(str_resp), std::cend(str_resp) };
    }

    zmq::message_t url_db_req_message_handler::get(boost::property_tree::ptree& parsed_req)
    {
        const auto id = parsed_req.get<common::db::url_id_t>("id");
        const auto url = std::string{ m_db.get(id).url() };
        return zmq::message_t(url.begin(), url.end());
    }

    zmq::message_t url_db_req_message_handler::insert(boost::property_tree::ptree& parsed_req)
    {
        const auto url = parsed_req.get<std::string>("url");
        LOG(INFO) << "url_db_req_message_handler insert: " << url;
        const auto id = m_db.insert(url);

        boost::property_tree::ptree tree;
        tree.put("url", url);
        tree.put("id", id);

        std::ostringstream oss;
        boost::property_tree::write_json(oss, tree);
        const auto str_resp = oss.str();

        return zmq::message_t{ std::cbegin(str_resp), std::cend(str_resp) };
    }

    zmq::message_t url_db_req_message_handler::update_state_as_processed(boost::property_tree::ptree& parsed_req)
    {
        const auto id = parsed_req.get<common::db::url_id_t>("id");
        LOG(INFO) << "url_db_req_message_handler update_state_as_processed: " << id;
        m_db.update_state_as_processed(id);
        return get_ok_message();
    }

    zmq::message_t url_db_req_message_handler::get_ok_message() const
    {
        const auto ok = string_view{ "ok" };
        return zmq::message_t{ std::cbegin(ok), std::cend(ok) };
    }
}
