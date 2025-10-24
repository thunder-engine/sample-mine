#pragma once

#include <nativebehaviour.h>

#include <actor.h>
#include <meshrender.h>
#include <meshcollider.h>
#include <transform.h>

#include <mesh.h>
#include <log.h>

#include "Blocks/GrassBlock.cpp"
#include "Blocks/VegetationBlock.cpp"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

class ChunkRenderer;

struct ChunkData {
    static uint64_t posToIndex(int32_t x, int32_t y) {
        uint64_t index = x;
        index += uint64_t(y) << 31;
        return index;
    }

    std::vector<uint32_t> blocks;
    int32_t x;
    int32_t y;
    ChunkRenderer *renderer = nullptr;
};

static std::unordered_map<uint64_t, ChunkData> s_chunks;

static const std::unordered_map<BlockType, SolidBlock*> s_blockTypes = {
    {BlockType::Stone, new SolidBlock},
    {BlockType::Grass, new GrassBlock},
    {BlockType::Dirt, new SolidBlock},
    {BlockType::Cobblestone, new SolidBlock},
    {BlockType::Sapling, new VegetationBlock},
    {BlockType::Planks, new SolidBlock},
    {BlockType::Bedrock, new SolidBlock},
    //FlowingWater
    //Water
    //FlowingLava
    //Lava
    {BlockType::Sand, new SolidBlock},
    {BlockType::Gravel, new SolidBlock},
    {BlockType::GoldOre, new SolidBlock},
    {BlockType::IronOre, new SolidBlock},
    {BlockType::CoalOre, new SolidBlock},
    {BlockType::Log, new GrassBlock},
    {BlockType::Leaves, new SolidBlock},
    //...........
    {BlockType::TallGrass, new VegetationBlock},
};

class ChunkRenderer : public NativeBehaviour {
    A_OBJECT(ChunkRenderer, NativeBehaviour, Components)

    ChunkData *m_chunkData;

    Mesh *m_chunkMesh;
    Mesh *m_solidMesh;
    Mesh *m_vegetationMesh;
    MeshCollider *m_collider;
    MeshRender *m_render;

public:
    ChunkRenderer() :
            m_chunkMesh(Engine::objectCreate<Mesh>("ChunkMesh")),
            m_solidMesh(Engine::objectCreate<Mesh>("SolidMesh")),
            m_vegetationMesh(Engine::objectCreate<Mesh>("VegetationMesh")),
            m_collider(nullptr),
            m_render(nullptr) {

        m_chunkMesh->makeDynamic();
    }

    void start() override {
        m_collider = getComponent<MeshCollider>();
        if (m_collider) {
            m_collider->setMesh(m_solidMesh);
        }

        m_render = getComponent<MeshRender>();
        if (m_render) {
            m_render->setMesh(m_chunkMesh);
        }
    }

    void setChunkData(ChunkData &data) {
        m_chunkData = &data;
        m_chunkData->renderer = this;

        RebuildChunk();
    }

    void RebuildChunk() {
        m_solidMesh->clear();
        m_vegetationMesh->clear();
        m_chunkMesh->clear();

        for (uint32_t y = 0; y < CHUNK_HEIGHT; y++) {
            for (uint32_t x = 0; x < CHUNK_WIDTH; x++) {
                for (uint32_t z = 0; z < CHUNK_WIDTH; z++) {
                    GenerateBlock(x, y, z);
                }
            }
        }

        if (m_collider) {
            m_collider->setMesh(m_solidMesh);
        }

        m_chunkMesh->batchMesh(*m_solidMesh);
        m_chunkMesh->batchMesh(*m_vegetationMesh);

        m_chunkMesh->recalcNormals();
        m_chunkMesh->recalcBounds();

        if (m_render) {
            m_render->setMesh(m_chunkMesh);
        }
    }

    void changeBlock(int32_t x, int32_t y, int32_t z, BlockType newType) {
        int chunkX = x / CHUNK_WIDTH;
        int chunkY = z / CHUNK_WIDTH;

        uint32_t x0 = x % CHUNK_WIDTH;
        uint32_t z0 = z % CHUNK_WIDTH;
        size_t index = x0 + CHUNK_WIDTH * (y * CHUNK_WIDTH + z0);

        if(m_chunkData && unpackType(m_chunkData->blocks[index]) != BlockType::Bedrock) {
            packType(m_chunkData->blocks[index], newType);

            RebuildChunk();

            if (x0 == 0) {
                auto it = s_chunks.find(ChunkData::posToIndex(chunkX - 1, chunkY));
                if (it != s_chunks.end()) {
                    it->second.renderer->RebuildChunk();
                }
            }
            else if (x0 == CHUNK_WIDTH - 1) {
                auto it = s_chunks.find(ChunkData::posToIndex(chunkX + 1, chunkY));
                if (it != s_chunks.end()) {
                    it->second.renderer->RebuildChunk();
                }
            }

            if (z0 == 0) {
                auto it = s_chunks.find(ChunkData::posToIndex(chunkX, chunkY - 1));
                if (it != s_chunks.end()) {
                    it->second.renderer->RebuildChunk();
                }
            }
            else if (z0 == CHUNK_WIDTH - 1) {
                auto it = s_chunks.find(ChunkData::posToIndex(chunkX, chunkY + 1));
                if (it != s_chunks.end()) {
                    it->second.renderer->RebuildChunk();
                }
            }
        }
    }

    static void packType(uint32_t& block, BlockType type) {
        block = (uint32_t)type;
    }

    static BlockType unpackType(uint32_t block) {
        return (BlockType)block;
    }

protected:
    void GenerateBlock(uint32_t x, uint32_t y, uint32_t z) {
        BlockType type = unpackType(GetBlockAtPosition(x, y, z));

        if (type == BlockType::Air) {
            return;
        }

        auto it = s_blockTypes.find(type);
        if (it != s_blockTypes.end()) {
            SolidBlock *block = it->second;
            uint8_t mask = 0;
            if (!isSolidBlock(unpackType(GetBlockAtPosition(x, y + 1, z)))) {
                mask |= SolidBlock::Top;
            }
            if (!isSolidBlock(unpackType(GetBlockAtPosition(x, y - 1, z)))) {
                mask |= SolidBlock::Bottom;
            }
            if (!isSolidBlock(unpackType(GetBlockAtPosition(x - 1, y, z)))) {
                mask |= SolidBlock::Left;
            }
            if (!isSolidBlock(unpackType(GetBlockAtPosition(x + 1, y, z)))) {
                mask |= SolidBlock::Right;
            }
            if (!isSolidBlock(unpackType(GetBlockAtPosition(x, y, z - 1)))) {
                mask |= SolidBlock::Back;
            }
            if (!isSolidBlock(unpackType(GetBlockAtPosition(x, y, z + 1)))) {
                mask |= SolidBlock::Front;
            }

            if(mask > 0) {
                block->buildGeometry(block->isCollidable() ? *m_solidMesh : *m_vegetationMesh, type, mask, x, y, z);
            }
        }

    }

    inline bool isSolidBlock(BlockType type) {
        return (type != BlockType::Air && type != BlockType::Leaves && type != BlockType::TallGrass && type != BlockType::Sapling);
    }

    uint32_t GetBlockAtPosition(int32_t x, int32_t y, int32_t z) {
        if (x > -1 && y > -1 && z > -1 && x < CHUNK_WIDTH && y < CHUNK_HEIGHT && z < CHUNK_WIDTH) {
            size_t index = x + CHUNK_WIDTH * (y * CHUNK_WIDTH + z);
            return m_chunkData->blocks[index];
        }

        if(y < 0) {
            return (uint32_t)BlockType::Dirt;
        }

        int32_t adjustX = m_chunkData->x;
        if (x < 0) {
            adjustX--;
            x += CHUNK_WIDTH;
        } else if (x >= CHUNK_WIDTH) {
            adjustX++;
            x -= CHUNK_WIDTH;
        }
        
        uint32_t adjustY = m_chunkData->y;
        if (z < 0) {
            adjustY--;
            z += CHUNK_WIDTH;
        } else if (z >= CHUNK_WIDTH) {
            adjustY++;
            z -= CHUNK_WIDTH;
        }
        auto it = s_chunks.find(ChunkData::posToIndex(adjustX, adjustY));
        if(it != s_chunks.end()) {
            size_t index = x + CHUNK_WIDTH * (y * CHUNK_WIDTH + z);
            return it->second.blocks[index];
        }

        return (uint32_t)BlockType::Dirt;
    }
};
