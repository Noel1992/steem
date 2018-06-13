#ifndef _STEEM_JSON_RPC2_STATE_
#define _STEEM_JSON_RPC2_STATE_

#include <boost/thread/shared_mutex.hpp>
#include <fc/thread/future.hpp>
#include <fc/variant.hpp>
#include <functional>
#include <atomic>

namespace fc
{
namespace rpc2
{
struct request
{
    std::string jsonrpc = "2.0";
    optional<uint64_t> id;
    std::string method;
    variants params;
};

struct error_object
{
    int64_t code;
    std::string message;
    optional<variant> data;
};

struct response
{
    response()
    {
    }
    response(int64_t i, fc::variant r)
        : id(i)
        , result(r)
    {
    }
    response(int64_t i, error_object r)
        : id(i)
        , error(r)
    {
    }
    int64_t id = 0;
    optional<fc::variant> result;
    optional<error_object> error;
};

class state
{
public:
    typedef std::function<variant(const variants&)> method;

    state();
    ~state();

public:
    void handle_reply(const response& response);

    request start_remote_call(const string& method_name, variants args);
    variant wait_for_response(uint64_t request_id);

    void close();

private:
    std::atomic<uint64_t> _next_id;
    std::unordered_map<uint64_t, fc::promise<variant>::ptr> _awaiting;
    mutable boost::shared_mutex _awaiting_mutex;
};
} // namespace rpc
} // namespace fc

FC_REFLECT(fc::rpc2::request, (jsonrpc)(id)(method)(params));
FC_REFLECT(fc::rpc2::error_object, (code)(message)(data))
FC_REFLECT(fc::rpc2::response, (id)(result)(error))

#endif // _STEEM_JSON_RPC2_STATE_