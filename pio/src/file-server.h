#pragma once
#include <ESP8266WebServer.h>
#include <FS.h>

class FileServer {
  private:
    /* data */
    ESP8266WebServer *server_;
    String appPath;
    void handleUpload();
    void handleList();
    void handleRemove();
    void handleRoot();
    void onNotFound();
    bool loadFromSPIFFS(String path, String dataType);

  public:
    FileServer(String appPath);
    void begin(ESP8266WebServer *server__);
};

extern FileServer fileServer;