#pragma once

#include <nativebehaviour.h>
#include <prefab.h>
#include <actor.h>
#include <scene.h>
#include <transform.h>
#include <log.h>
#include "ChunkRenderer.cpp"

#define SIZE 7

class WorldManager : public NativeBehaviour {
    A_OBJECT(WorldManager, NativeBehaviour, Components)

    A_PROPERTIES(
        A_PROPERTY(Prefab *, chunkPrefab, WorldManager::chunkPrefab, WorldManager::setChunkPrefab),
        A_PROPERTY(Prefab *, playerPrefab, WorldManager::playerPrefab, WorldManager::setPlayerPrefab)
    )

    Prefab *m_chunkPrefab = nullptr;
    Prefab *m_playerPrefab = nullptr;

public:
    // Use this to initialize behaviour
    void start() override {
        s_chunks.clear();

        if(m_chunkPrefab) {
            // Generate world
            for(int x = 0; x < SIZE; x++) {
                for(int y = 0; y < SIZE; y++) {
                    s_chunks[ChunkData::posToIndex(x, y)] = generateChunk(x, y);
                }
            }

            // Generate structures
            for(int x = 0; x < SIZE; x++) {
                for(int y = 0; y < SIZE; y++) {
                    generateStructures(x, y);
                }
            }

            // Set world to render
            for(int x = 0; x < SIZE; x++) {
                for(int y = 0; y < SIZE; y++) {
                    Actor* object = static_cast<Actor*>(m_chunkPrefab->actor()->clone(actor()));
                    object->transform()->setPosition(Vector3(x * CHUNK_WIDTH, 0.0f, y * CHUNK_WIDTH));
                    ChunkRenderer* chunk = object->getComponent<ChunkRenderer>();
                    if(chunk) {
                        chunk->setChunkData(s_chunks[ChunkData::posToIndex(x, y)]);
                    }
                }
            }

            // Spawn player
            if(m_playerPrefab) {
                Actor *object = static_cast<Actor *>(m_playerPrefab->actor()->clone(actor()->scene()));
                int32_t xz = (CHUNK_WIDTH * SIZE) / 2;
                int chunkXZ = xz / CHUNK_WIDTH;
                ChunkData &data = s_chunks[ChunkData::posToIndex(chunkXZ, chunkXZ)];

                uint32_t xz0 = xz % CHUNK_WIDTH;
                int y = CHUNK_HEIGHT-1;
                while(ChunkRenderer::unpackType(data.blocks[xz0 + CHUNK_WIDTH * (y * CHUNK_WIDTH + xz0)]) == BlockType::Air) {
                    y--;
                }
                object->transform()->setPosition(Vector3(xz, y+3, xz));
            }
        }
    }

    // Will be called each frame. Use this to write your game logic
    void update() override {

    }
    
    static void changeBlock(int32_t x, int32_t y, int32_t z, BlockType newType) {
        int chunkX = x / CHUNK_WIDTH;
        int chunkY = z / CHUNK_WIDTH;

        auto it = s_chunks.find(ChunkData::posToIndex(chunkX, chunkY));
        
        if (it != s_chunks.end()) {
            it->second.renderer->changeBlock(x, y, z, newType);
        }
    }

    Prefab *chunkPrefab() const {
        return m_chunkPrefab;
    }
    
    void setChunkPrefab(Prefab *prefab) {
        m_chunkPrefab = prefab;
    }

    Prefab *playerPrefab() const {
        return m_playerPrefab;
    }
    
    void setPlayerPrefab(Prefab *prefab) {
        m_playerPrefab = prefab;
    }

    static ChunkData generateChunk(int32_t posX, int32_t posY) {
        ChunkData result;
        result.blocks.resize(CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT);
        for(int32_t x = 0; x < CHUNK_WIDTH; x++) {
            for(int32_t z = 0; z < CHUNK_WIDTH; z++) {
                int height = Mathf::perlinNoise((x + posX * CHUNK_WIDTH) * 0.1f, (z + posY * CHUNK_WIDTH) * 0.1f) * 20.0f + 20.0f;
                for(int32_t y = 0; y < height; y++) {
                    BlockType type = BlockType::Bedrock;
                    if(y > 2) {
                        if(y > height-3) {
                            type = BlockType::Dirt;
                            if(y == height-1) {
                                type = BlockType::Grass;

                                if(y < CHUNK_HEIGHT-1 && (rand() % 10) == 0) {
                                    size_t index = x + CHUNK_WIDTH * ((y+1) * CHUNK_WIDTH + z);
                                    ChunkRenderer::packType(result.blocks[index], (rand() % 30 == 0) ? BlockType::Sapling : BlockType::TallGrass);
                                }
                            }
                        } else {
                            type = BlockType::Stone;
                        }
                    }

                    ChunkRenderer::packType(result.blocks[x + CHUNK_WIDTH * (y * CHUNK_WIDTH + z)], type);
                    result.x = posX;
                    result.y = posY;
                }
            }
        }
        return result;
    }

    static void generateStructures(int32_t posX, int32_t posY) {
        ChunkData &data = s_chunks[ChunkData::posToIndex(posX, posY)];

        for(int32_t x = 0; x < CHUNK_WIDTH; x++) {
            for(int32_t z = 0; z < CHUNK_WIDTH; z++) {
                for(int32_t y = 0; y < CHUNK_HEIGHT; y++) {
                    size_t index = x + CHUNK_WIDTH * (y * CHUNK_WIDTH + z);

                    if(ChunkRenderer::unpackType(data.blocks[index]) == BlockType::Sapling) {
                        generateTree(data.x * CHUNK_WIDTH + x, y, data.y * CHUNK_WIDTH + z);
                    }
                }
            }
        }

    }

    static void generateTree(int32_t x, int32_t y, int32_t z) {
        int chunkX = x / CHUNK_WIDTH;
        int chunkY = z / CHUNK_WIDTH;

        uint32_t x0 = x % CHUNK_WIDTH;
        uint32_t z0 = z % CHUNK_WIDTH;

        int32_t height = 7;

        if(height == 0) {
            return;
        }

        // Leaves
        for(int32_t radius = 1; radius < 4; radius++) {
            for(int32_t i = -radius; i <= radius; i++) {
                for(int32_t j = -radius; j <= radius; j++) {
                    float distance = sqrt(i*i + j*j)-0.2f;
                    if(distance < radius) {
                        if((x + i) < 0 || (z + j) < 0) {
                            continue;
                        }
                        int32_t chunkX = (x + i) / CHUNK_WIDTH;
                        int32_t chunkY = (z + j) / CHUNK_WIDTH;

                        auto it = s_chunks.find(ChunkData::posToIndex(chunkX, chunkY));
                        if(it != s_chunks.end())  {
                            uint32_t x0 = (x + i) % CHUNK_WIDTH;
                            uint32_t z0 = (z + j) % CHUNK_WIDTH;

                            size_t index = x0 + CHUNK_WIDTH * ((y + height - radius) * CHUNK_WIDTH + z0);
                            if(it->second.blocks[index] == (uint32_t)BlockType::Air) {
                                ChunkRenderer::packType(it->second.blocks[index], BlockType::Leaves);
                            }
                        }
                    }
                }
            }
        }

        // Trunk
        ChunkData &data = s_chunks[ChunkData::posToIndex(chunkX, chunkY)];
        for(int32_t i = 0; i < height-1; i++) {
            size_t index = x0 + CHUNK_WIDTH * ((y + i) * CHUNK_WIDTH + z0);
            ChunkRenderer::packType(data.blocks[index], BlockType::Log);
        }

    }

};
