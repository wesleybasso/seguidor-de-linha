#include "web_server.h"
#include "web_ui.h"
#include "api_routes.h"
#include <WebServer.h>

static WebServer server(80);

void web_server_begin(pegasus_telemetry_t &telemetry, pegasus_config_t &config, run_summary_t &summary) {
    server.on("/", HTTP_GET, []() {
        server.send_P(200, "text/html", PEGASUS_WEB_UI);
    });
    register_api_routes(server, telemetry, config, summary);
    server.begin();
}

void web_server_loop() {
    server.handleClient();
}
