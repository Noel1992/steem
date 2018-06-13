#include <fc/reflect/variant.hpp>
#include <fc/rpc2/state.hpp>
#include <fc/thread/thread.hpp>

namespace fc
{
namespace rpc2
{

state::state()
:_next_id(std::atomic<int>(1))
{

}

state::~state()
{
    close();
}

void state::handle_reply(const response& response)
{
    boost::shared_lock<boost::shared_mutex> lock(_awaiting_mutex);

    auto await = _awaiting.find(response.id);
    FC_ASSERT(await != _awaiting.end(), "Unknown Response ID: ${id}",
              ("id", response.id)("response", response));
    if (response.result)
    {
        await->second->set_value(*response.result);
    }
    else if (response.error)
    {
        await->second->set_exception(exception_ptr(new FC_EXCEPTION(
            exception, "${error}",
            ("error", response.error->message)("data", response))));
    }
    else
    {
        await->second->set_value(fc::variant());
    }
    _awaiting.erase(await);
}

request state::start_remote_call(const string& method_name, variants args)
{
    request request{"2.0", _next_id++, method_name, std::move(args)};

    boost::unique_lock<boost::shared_mutex> lock(_awaiting_mutex);

    _awaiting[*request.id] = fc::promise<variant>::ptr(
        new fc::promise<variant>("json_connection::async_call"));
    return request;
}

variant state::wait_for_response(uint64_t request_id)
{
    boost::shared_lock<boost::shared_mutex> lock(_awaiting_mutex);

    auto itr = _awaiting.find(request_id);
    FC_ASSERT(itr != _awaiting.end());
    return fc::future<variant>(itr->second).wait();
}

void state::close()
{
    boost::unique_lock<boost::shared_mutex> lock(_awaiting_mutex);

    for (auto item : _awaiting)
    {
        item.second->set_exception(fc::exception_ptr(
            new FC_EXCEPTION(eof_exception, "connection closed")));
    }
    _awaiting.clear();
}

} // namespace rpc2
} // namespace fc
