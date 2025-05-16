#include "RequestHandler.h"
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <map>

std::string get404Page() {
    std::ifstream file("static/404.html");
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
    return "<html><body><h1>404 Not Found</h1></body></html>";
}


void sendResponse(int fd, const std::string& content, const std::string& contentType = "text/html") {
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.size()) +
                           "\r\nContent-Type: " + contentType + "\r\n\r\n" + content;
    if (send(fd, response.c_str(), response.size(), 0) == -1) {
        std::cerr << "[ERROR] Failed to send response via socket" << std::endl;
    } else {
        std::cout << "[INFO] Response sent via socket (" << content.size() << " bytes)" << std::endl;
    }
}

void sendResponse(SSL* ssl, const std::string& content, const std::string& contentType = "text/html") {
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.size()) +
                           "\r\nContent-Type: " + contentType + "\r\n\r\n" + content;
    if (SSL_write(ssl, response.c_str(), response.size()) <= 0) {
        std::cerr << "[ERROR] Failed to send response via SSL" << std::endl;
    } else {
        std::cout << "[INFO] Response sent via SSL (" << content.size() << " bytes)" << std::endl;
    }
}

std::string urldecode(const std::string& str) {
    std::string result;
    int i, j;
    for (i = 0; i < str.length(); i++) {
        if (str[i] == '%') {
            sscanf(str.substr(i + 1, 2).c_str(), "%2x", &j);
            result += static_cast<char>(j);
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string parsePostData(const std::string& data) {
    std::map<std::string, std::string> params;
    std::istringstream ss(data);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
        size_t eq = pair.find('=');
        if (eq != std::string::npos) {
            std::string key = pair.substr(0, eq);
            std::string val = pair.substr(eq + 1);
            params[key] = urldecode(val);
        }
    }

    std::string name = params.count("name") ? params["name"] : "Guest";

    std::cout << "[INFO] Parsed POST data: name = " << name << std::endl;

    std::string html = "<!DOCTYPE html>"
        "<html lang='en'>"
        "<head>"
        "    <meta charset='UTF-8'>"
        "    <title>Welcome</title>"
        "    <style>"
        "        body {"
        "            margin: 0;"
        "            padding: 0;"
        "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
        "            background: linear-gradient(to right, #fceabb, #f8b500);"
        "            display: flex;"
        "            align-items: center;"
        "            justify-content: center;"
        "            height: 100vh;"
        "        }"
        "        .container {"
        "            background-color: #ffffff;"
        "            padding: 30px 40px;"
        "            border-radius: 15px;"
        "            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.2);"
        "            text-align: center;"
        "            max-width: 400px;"
        "            width: 100%;"
        "        }"
        "        h1 {"
        "            color: #2c3e50;"
        "            margin-bottom: 10px;"
        "        }"
        "        p {"
        "            color: #444;"
        "            font-size: 16px;"
        "        }"
        "        a {"
        "            display: inline-block;"
        "            margin-top: 20px;"
        "            text-decoration: none;"
        "            background-color: #3498db;"
        "            color: white;"
        "            padding: 10px 20px;"
        "            border-radius: 8px;"
        "            transition: background-color 0.3s ease;"
        "        }"
        "        a:hover {"
        "            background-color: #2980b9;"
        "        }"
        "    </style>"
        "</head>"
        "<body>"
        "    <div class='container'>"
        "        <h1>Hello, " + name + "!</h1>"
        "        <p>Your POST request was received successfully.</p>"
        "        <a href='/'>← Back to form</a>"
        "    </div>"
        "</body>"
        "</html>";


    return html;
}

void handleRawRequest(const std::string& request, int fd, SSL* ssl = nullptr) {
    std::istringstream req(request);
    std::string method, path;
    req >> method >> path;

    std::cout << "[INFO] Received request: method = " << method << ", path = " << path << std::endl;
    std::string notFound = get404Page();
    
    if (method == "GET") {
        std::string filePath = "static" + (path == "/" ? "/index.html" : path);
        std::ifstream file(filePath, std::ios::binary);
        if (file.is_open()) {
            std::ostringstream ss;
            ss << file.rdbuf();
            std::string content = ss.str();

            std::string contentType = "text/html";
            if (filePath.ends_with(".png")) contentType = "image/png";
            else if (filePath.ends_with(".jpg") || filePath.ends_with(".jpeg")) contentType = "image/jpeg";
            else if (filePath.ends_with(".css")) contentType = "text/css";
            else if (filePath.ends_with(".js")) contentType = "application/javascript";

            if (ssl)
                sendResponse(ssl, content, contentType);
            else
                sendResponse(fd, content, contentType);
        } else {
            if (ssl)
                sendResponse(ssl, notFound);
            else
                sendResponse(fd, notFound);
        }
    } else if (method == "POST") {
        if (path == "/submit") {
            std::string postData = request.substr(request.find("\r\n\r\n") + 4);
            std::string html = parsePostData(postData);
            if (ssl)
                sendResponse(ssl, html);
            else
                sendResponse(fd, html);
        } else {
            // Неверный POST путь — отдаем 404
            if (ssl)
                sendResponse(ssl, notFound);
            else
                sendResponse(fd, notFound);
        }
    } else {
        std::string notFound = get404Page();
        if (ssl)
            sendResponse(ssl, notFound);
        else
            sendResponse(fd, notFound);
    }
}

void handleRequest(int client_fd) {
    std::cout << "[INFO] Handling HTTP connection (fd = " << client_fd << ")" << std::endl;
    char buffer[8192] {};
    int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes > 0) {
        std::cout << "[DEBUG] Received " << bytes << " bytes from socket" << std::endl;
        handleRawRequest(std::string(buffer, bytes), client_fd);
    } else {
        std::cerr << "[ERROR] Failed to receive data from client_fd = " << client_fd << std::endl;
    }
}

void handleRequest(SSL* ssl) {
    std::cout << "[INFO] Handling HTTPS connection" << std::endl;
    char buffer[8192] {};
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));
    if (bytes > 0) {
        std::cout << "[DEBUG] Received " << bytes << " bytes via SSL" << std::endl;
        handleRawRequest(std::string(buffer, bytes), -1, ssl);
    } else {
        std::cerr << "[ERROR] Failed to receive data via SSL" << std::endl;
    }
}