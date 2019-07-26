#include <cstdlib>

#include <improbable/standard_library.h>
#include <improbable/worker.h>
#include <shoveler.h>

extern "C" {
#include <glib.h>

#include <shoveler/constants.h>
#include <shoveler/file.h>
#include <shoveler/log.h>
}

using improbable::EntityAcl;
using improbable::Metadata;
using improbable::Persistence;
using improbable::Position;
using improbable::WorkerAttributeSet;
using improbable::WorkerRequirementSet;
using shoveler::Bootstrap;
using shoveler::Client;
using shoveler::Color;
using shoveler::Drawable;
using shoveler::DrawableType;
using shoveler::Light;
using shoveler::LightType;
using shoveler::Material;
using shoveler::MaterialType;
using shoveler::Model;
using shoveler::PolygonMode;
using worker::ComponentRegistry;
using worker::Components;
using worker::Entity;
using worker::EntityId;
using worker::List;
using worker::Map;
using worker::Option;
using worker::Result;
using worker::SnapshotOutputStream;
using worker::StreamErrorCode;

int main(int argc, char **argv) {
	shovelerLogInit("shoveler-spatialos/workers/cmake/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);

	if (argc != 2) {
		shovelerLogError("Usage:\n\t%s <seed snapshot file>", argv[0]);
		return EXIT_FAILURE;
	}

	std::string filename(argv[1]);

	const ComponentRegistry& components = Components<
		Bootstrap,
		Client,
		Drawable,
		EntityAcl,
		Light,
		Material,
		Metadata,
		Model,
		Persistence,
		Position>{};

	Color grayColor{0.7f, 0.7f, 0.7f};
	Color whiteColor{1.0f, 1.0f, 1.0f};

	WorkerAttributeSet clientAttributeSet({"client"});
	WorkerAttributeSet serverAttributeSet({"server"});
	WorkerRequirementSet clientRequirementSet({clientAttributeSet});
	WorkerRequirementSet serverRequirementSet({serverAttributeSet});
	WorkerRequirementSet clientOrServerRequirementSet({clientAttributeSet, serverAttributeSet});

	std::unordered_map<EntityId, Entity> entities;

	Entity bootstrapEntity;
	bootstrapEntity.Add<Metadata>({"bootstrap"});
	bootstrapEntity.Add<Persistence>({});
	bootstrapEntity.Add<Position>({{0, 0, 0}});
	bootstrapEntity.Add<Bootstrap>({});
	Map<std::uint32_t, WorkerRequirementSet> bootstrapComponentAclMap;
	bootstrapComponentAclMap.insert({{Bootstrap::ComponentId, serverRequirementSet}});
	bootstrapEntity.Add<EntityAcl>({clientOrServerRequirementSet, bootstrapComponentAclMap});
	entities[1] = bootstrapEntity;

	Entity cubeDrawableEntity;
	cubeDrawableEntity.Add<Metadata>({"drawable"});
	cubeDrawableEntity.Add<Persistence>({});
	cubeDrawableEntity.Add<Position>({{0, 0, 0}});
	cubeDrawableEntity.Add<Drawable>({DrawableType::CUBE});
	cubeDrawableEntity.Add<EntityAcl>({clientOrServerRequirementSet, {}});
	EntityId cubeDrawableEntityId = 2;
	entities[cubeDrawableEntityId] = cubeDrawableEntity;

	Entity quadDrawableEntity;
	quadDrawableEntity.Add<Metadata>({"drawable"});
	quadDrawableEntity.Add<Persistence>({});
	quadDrawableEntity.Add<Position>({{0, 0, 0}});
	quadDrawableEntity.Add<Drawable>({DrawableType::QUAD});
	quadDrawableEntity.Add<EntityAcl>({clientOrServerRequirementSet, {}});
	EntityId quadDrawableEntityId = 3;
	entities[quadDrawableEntityId] = quadDrawableEntity;

	Entity pointDrawableEntity;
	pointDrawableEntity.Add<Metadata>({"drawable"});
	pointDrawableEntity.Add<Persistence>({});
	pointDrawableEntity.Add<Position>({{0, 0, 0}});
	pointDrawableEntity.Add<Drawable>({DrawableType::POINT});
	pointDrawableEntity.Add<EntityAcl>({clientOrServerRequirementSet, {}});
	EntityId pointDrawableEntityId = 4;
	entities[pointDrawableEntityId] = pointDrawableEntity;

	Entity grayColorMaterialEntity;
	grayColorMaterialEntity.Add<Metadata>({"material"});
	grayColorMaterialEntity.Add<Persistence>({});
	grayColorMaterialEntity.Add<Position>({{0, 0, 0}});
	grayColorMaterialEntity.Add<Material>({MaterialType::COLOR, grayColor, {}, {}, {}});
	grayColorMaterialEntity.Add<EntityAcl>({clientOrServerRequirementSet, {}});
	EntityId grayColorMaterialEntityId = 5;
	entities[grayColorMaterialEntityId] = grayColorMaterialEntity;

	Entity whiteParticleMaterialEntity;
	whiteParticleMaterialEntity.Add<Metadata>({"material"});
	whiteParticleMaterialEntity.Add<Persistence>({});
	whiteParticleMaterialEntity.Add<Position>({{0, 0, 0}});
	whiteParticleMaterialEntity.Add<Material>({MaterialType::PARTICLE, whiteColor, {}, {}, {}});
	whiteParticleMaterialEntity.Add<EntityAcl>({clientOrServerRequirementSet, {}});
	EntityId whiteParticleMaterialEntityId = 6;
	entities[whiteParticleMaterialEntityId] = whiteParticleMaterialEntity;

	Entity planeEntity;
	planeEntity.Add<Metadata>({"plane"});
	planeEntity.Add<Persistence>({});
	planeEntity.Add<Position>({{0, -2, 0}});
	planeEntity.Add<Model>({quadDrawableEntityId, grayColorMaterialEntityId, {SHOVELER_PI / 2.0f, 0.0f, 0.0f}, {25.0f, 25.0f, 1.0f}, true, false, false, true, PolygonMode::FILL});
	planeEntity.Add<EntityAcl>({clientRequirementSet, {}});
	entities[7] = planeEntity;

	Entity cubeEntity;
	cubeEntity.Add<Metadata>({"cube"});
	cubeEntity.Add<Persistence>({});
	cubeEntity.Add<Position>({{0, 0, 5}});
	cubeEntity.Add<Model>({cubeDrawableEntityId, grayColorMaterialEntityId, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, true, false, false, true, PolygonMode::FILL});
	cubeEntity.Add<EntityAcl>({clientRequirementSet, {}});
	entities[8] = cubeEntity;

	Entity lightEntity;
	lightEntity.Add<Metadata>({"light"});
	lightEntity.Add<Persistence>({});
	lightEntity.Add<Position>({{-1, 5, -1}});
	lightEntity.Add<Model>({pointDrawableEntityId, whiteParticleMaterialEntityId, {0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.0f}, true, true, false, false, PolygonMode::FILL});
	lightEntity.Add<Light>({LightType::POINT, 1024, 1024, 1, 0.01f, 80.0f, {1.0f, 1.0f, 1.0f}, {}});
	lightEntity.Add<EntityAcl>({clientRequirementSet, {}});
	entities[9] = lightEntity;

	Result<SnapshotOutputStream, StreamErrorCode> outputStream = SnapshotOutputStream::Create(components, filename);
	if(!outputStream) {
		shovelerLogError("Failed to open snapshot stream: %s", outputStream.GetErrorMessage().c_str());
		return EXIT_FAILURE;
	}

	for(std::unordered_map<EntityId, Entity>::const_iterator iter = entities.begin(); iter != entities.end(); ++iter) {
		Result<worker::None, StreamErrorCode> entityWritten = outputStream->WriteEntity(iter->first, iter->second);
		if(!entityWritten) {
			shovelerLogError("Failed to write entity %lld to snapshot: %s", entityWritten.GetErrorMessage().c_str());
			return EXIT_FAILURE;
		}
	}

	shovelerLogInfo("Successfully wrote snapshot with %zu entities.", entities.size());
	return EXIT_SUCCESS;
}
