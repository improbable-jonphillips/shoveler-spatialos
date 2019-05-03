#ifndef SHOVELER_CLIENT_POSITION_H
#define SHOVELER_CLIENT_POSITION_H

#include <improbable/worker.h>

extern "C" {
#include <shoveler/view.h>
#include <shoveler/types.h>
}

void registerPositionCallbacks(worker::Connection& connection, worker::Dispatcher& dispatcher, ShovelerView *view, ShovelerCoordinateMapping mappingX, ShovelerCoordinateMapping mappingY, ShovelerCoordinateMapping mappingZ);
ShovelerVector3 getEntitySpatialOsPosition(ShovelerView *view, worker::EntityId entityId);

#endif
