#ifndef _STEEM_JSON_RPC_2_CLIENT_
#define _STEEM_JSON_RPC_2_CLIENT_

#include <fc/io/json.hpp>
#include <fc/network/http/websocket.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/rpc/api_connection.hpp>
#include <fc/rpc2/state.hpp>

namespace fc
{
namespace rpc2
{

class client
{
public:
    client(fc::http::websocket_connection& c);
    ~client();

    variant send_call(string method_name, variants args = variants());

protected:
    std::string on_message(const std::string& message, bool send_message = true);

private:
    void closed();

private:
    fc::http::websocket_connection& _connection;
    fc::rpc::state _rpc_state;
};

}; // namespace rpc2
}; // namespace fc

#endif // _STEEM_JSON_RPC_2_CLIENT_