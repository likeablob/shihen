#include "file-server.h"

File fsUploadFile;
FileServer fileServer("/app/");

FileServer::FileServer(String path) : appPath(path) {}

void FileServer::begin(ESP8266WebServer *server__) {
    server_ = server__;

    server_->serveStatic(appPath.c_str(), SPIFFS, appPath.c_str(),
                         "max-age=31556926");
    server_->on("/", [&]() { handleRoot(); });

    server_->on("/cp", HTTP_POST,
                [&]() { server_->send(200, "text/plain", ""); },
                [&]() { handleUpload(); });
    server_->on("/ls", HTTP_GET, [&]() { handleList(); });
    server_->on("/rm", HTTP_GET, [&]() { handleRemove(); });
    server_->onNotFound([&]() { onNotFound(); });
}

void FileServer::handleRoot() {
    server_->sendHeader("Location", appPath);
    server_->sendHeader("Cache-Control", "max-age=31556926");
    server_->send(301);
}

void FileServer::handleUpload() {
    // if (server_->uri() != "/cp") return;
    HTTPUpload &upload = server_->upload();
    if(upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if(!filename.startsWith("/"))
            filename = "/" + filename;
        fsUploadFile = SPIFFS.open(filename, "w");
        filename = String();
    } else if(upload.status == UPLOAD_FILE_WRITE) {
        if(fsUploadFile)
            fsUploadFile.write(upload.buf, upload.currentSize);
    } else if(upload.status == UPLOAD_FILE_END) {
        if(fsUploadFile)
            fsUploadFile.close();
    }
}

void FileServer::handleList() {
    String path = server_->arg("path");
    if(!server_->hasArg("path")) {
        path = "/";
    }

    Dir dir = SPIFFS.openDir(path);

    String output = "[";
    while(dir.next()) {
        File entry = dir.openFile("r");
        if(output != "[")
            output += ',';
        bool isDir = false;
        output += "{\"type\":\"";
        output += (isDir) ? "dir" : "file";
        output += "\",\"name\":\"";
        output += String(entry.name()).substring(1);
        output += "\",\"size\":\"";
        output += String(entry.size());
        output += "\"}";
        entry.close();
    }

    output += "]";
    server_->sendHeader("Access-Control-Allow-Origin", "*");
    server_->sendHeader("Access-Control-Allow-Methods", "*");
    server_->send(200, "text/json", output);
}

void FileServer::handleRemove() {
    if(!server_->hasArg("path")) {
        server_->send(500, "text/plain", "BAD ARGS");
        return;
    }

    String path = server_->arg("path");
    bool res = SPIFFS.remove(path);
    if(res) {
        server_->send(200, "text/json", "{\"res\":0}");
    } else {
        server_->send(500, "text/json", "{\"res\":1}");
    }
}

bool FileServer::loadFromSPIFFS(String path, String dataType) {
    if(path.endsWith(".css")) {
        dataType = "text/css";
    }
    // if(path.endsWith(".js.gz")){
    //   dataType = "application/javascript";
    // }
    if(path.endsWith(".js")) {
        dataType = "application/javascript";
    }

    File f = SPIFFS.open(path.c_str(), "r");
    if(!f)
        return false;
    server_->streamFile(f, dataType);
    f.close();
    return true;
}

void FileServer::onNotFound() {
    if(loadFromSPIFFS(server_->uri(), "application/octet-stream"))
        return;
    String message = "404: No such file\n\n";
    message += "URI: ";
    message += server_->uri();
    message += "\nMethod: ";
    message += (server_->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server_->args();
    message += "\n";
    for(uint8_t i = 0; i < server_->args(); i++) {
        message += " NAME:" + server_->argName(i) +
                   "\n VALUE:" + server_->arg(i) + "\n";
    }
    server_->send(404, "text/plain", message);
}
