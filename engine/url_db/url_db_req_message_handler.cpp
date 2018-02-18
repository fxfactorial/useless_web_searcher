#include "url_db_req_message_handler.hpp"

#include "url_db/url_db.hpp"

#include <boost/property_tree/json_parser.hpp>

#include <easylogging/easylogging++.h>

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

        namespace pt = boost::property_tree;

        pt::ptree tree;

        pt::read_json(iss, tree);

        const auto msg_type = tree.get<std::string>("type");

        if(msg_type == "insert")
        {
            return insert(tree.get<std::string>("url"));
        }

        return zmq::message_t();
    }

    zmq::message_t url_db_req_message_handler::get_for_processing(boost::property_tree::ptree& parsed_req)
    {
        return zmq::message_t();
    }

    zmq::message_t url_db_req_message_handler::get(boost::property_tree::ptree& parsed_req)
    {
        return zmq::message_t();
    }

    zmq::message_t url_db_req_message_handler::insert(boost::property_tree::ptree& parsed_req)
    {
        const auto url = parsed_req.get<std::string>("url");

        m_db.insert(url);

        const auto ok = "ok";

        return zmq::message_t{ std::begin(ok), std::end(ok) };
    }
}
