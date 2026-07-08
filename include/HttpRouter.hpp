#ifndef HTTP_ROUTER_HPP
# define HTTP_ROUTER_HPP

# include "webserver.hpp"

class HttpRouter {
    public:
        HttpRouter();
        HttpRouter(const HttpRouter& other);
        HttpRouter& operator=(const HttpRouter& other);
        ~HttpRouter();

        const RouteConfig* matchRoute(const std::string& uri, const Server* server) const;
};

#endif