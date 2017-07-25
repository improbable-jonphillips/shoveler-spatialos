#include <improbable/worker.h>

extern "C" {
#include <camera/identity.h>
#include <game.h>
#include <log.h>
}

static void updateGame(ShovelerGame *game, double dt);

int main(int argc, char **argv) {
	if (argc != 4) {
		return 1;
	}

	const char *windowTitle = "ShovelerClient";
	bool fullscreen = false;
	bool vsync = true;
	int samples = 4;
	int width = 640;
	int height = 480;

	ShovelerGame *game = shovelerGameCreate(windowTitle, width, height, samples, fullscreen, vsync);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	worker::ConnectionParameters parameters;
	parameters.WorkerType = "ShovelerClient";
	parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
	parameters.Network.UseExternalIp = false;

	const std::string workerId = argv[1];
	const std::string hostname = argv[2];
	const std::uint16_t port = static_cast<std::uint16_t>(std::stoi(argv[3]));

	shovelerLogInfo("Connecting as worker %s to %s:%d.", workerId.c_str(), hostname.c_str(), port);
	worker::Connection connection = worker::Connection::ConnectAsync(hostname, port, workerId, parameters).Get();
	bool disconnected = false;
	worker::Dispatcher dispatcher;

	dispatcher.OnDisconnect([&](const worker::DisconnectOp& op) {
		shovelerLogError("Disconnected from SpatialOS: %s", op.Reason.c_str());
		disconnected = true;
	});


	game->camera = shovelerCameraIdentityCreate();
	game->scene = shovelerSceneCreate();
	game->update = updateGame;

	while(shovelerGameIsRunning(game) && !disconnected) {
		dispatcher.Process(connection.GetOpList(0));
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerCameraFree(game->camera);
	shovelerSceneFree(game->scene);

	shovelerGameFree(game);
}

static void updateGame(ShovelerGame *game, double dt)
{

}
