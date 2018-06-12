#include <iostream>
#include <vector>
#include <string>
#include <fc/rpc2/apiclient.hpp>

void show_result(fc::variant_object& var)
{
    auto it = var.begin();
    for (; it != var.end(); it++)
    {
        if (it->value().is_string())
        {
            std::cout << it->key() << ": " << it->value().as_string() << std::endl;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: jsonrpc2 websocketaddress" << std::endl;
        return 0;
    }

    const char* wsaddr = argv[1];
    std::cout << "ws addr is: " << wsaddr << std::endl;
    try
    {
        fc::rpc2::apiclient* cli = new fc::rpc2::apiclient(wsaddr);
        std::vector<std::string> args;
        args.push_back("wb-100");
        args.push_back("wb-200");
        auto accounts = cli->call("condenser_api.get_accounts", args);
        auto var_arr = accounts.get_array();
        for (size_t i = 0; i < var_arr.size(); ++i)
        {
            std::cout << "\n===>>> object [" << i << "] <<<===\n" << std::endl;
            show_result(var_arr[i].get_object());
        }
        delete cli;
    }
    catch (const fc::assert_exception& e)
    {
        std::cerr << e.to_string() << "\n";
    }
    catch (const fc::exception& e)
    {
        std::cerr << e.to_string() << "\n";
    }
    catch(...)
    {
        std::cout << "other exception" << std::endl;
    }
    return 0;
}