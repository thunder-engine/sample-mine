#pragma once

#include <mesh.h>

enum class BlockType {
    Air,
    Stone,
    Grass,
    Dirt,
    Cobblestone,
    Planks,
    Sapling,
    Bedrock,
    FlowingWater,
    Water,
    FlowingLava,
    Lava,
    Sand,
    Gravel,
    GoldOre,
    IronOre,
    CoalOre,
    Log,
    Leaves,
    Sponge,
    Glass,
    LapisOre,
    LapisBlock,
    Dispenser,
    Sandstone,
    Web = 30,
    TallGrass,
    DeadBush,
    Torch = 50,
    Vines = 106
};

class SolidBlock {
public:
    enum Sides {
        Top = (1 << 0),
        Bottom = (1 << 1),
        Left = (1 << 2),
        Right = (1 << 3),
        Back = (1 << 4),
        Front = (1 << 5)
    };

    const float tileMapWidth = 256.0f;
    const float tileMapHeight = 256.0f;

    const float tileWidth = 16.0f / tileMapWidth;
    const float tileHeight = 16.0f / tileMapWidth;

public:
    virtual bool isCollidable() const {
        return true;
    }

    virtual void buildGeometry(Mesh &mesh, BlockType type, int8_t mask, int32_t x, int32_t y, int32_t z) {
        int8_t count = 0;
        int8_t localMask = mask;
        for (; localMask; count++) {
            localMask &= (localMask - 1);
        }

        Vector3Vector vertices;
        vertices.resize(count * 4);

        Vector2Vector uvs;
        uvs.resize(count * 4);
        
        Vector4Vector colors = Vector4Vector(count * 4, Vector4(1.0f));
        
        uint32_t v = 0;
        uint32_t i = mesh.vertices().size();

        if (mask & Top) {
            vertices[v].x = x;
            vertices[v].y = y + 1;
            vertices[v].z = z;

            vertices[v + 1].x = x;
            vertices[v + 1].y = y + 1;
            vertices[v + 1].z = z + 1;

            vertices[v + 2].x = x + 1;
            vertices[v + 2].y = y + 1;
            vertices[v + 2].z = z;

            vertices[v + 3].x = x + 1;
            vertices[v + 3].y = y + 1;
            vertices[v + 3].z = z + 1;

            GenerateUvs(uvs, type, Top, v);

            v += 4;
            i += 4;

            addIndices(mesh.indices(), i);
        }
        if (mask & Bottom) {
            vertices[v].x = x;
            vertices[v].y = y;
            vertices[v].z = z;

            vertices[v + 1].x = x + 1;
            vertices[v + 1].y = y;
            vertices[v + 1].z = z;

            vertices[v + 2].x = x;
            vertices[v + 2].y = y;
            vertices[v + 2].z = z + 1;

            vertices[v + 3].x = x + 1;
            vertices[v + 3].y = y;
            vertices[v + 3].z = z + 1;

            GenerateUvs(uvs, type, Bottom, v);

            v += 4;
            i += 4;

            addIndices(mesh.indices(), i);
        }
        if (mask & Left) {
            vertices[v].x = x;
            vertices[v].y = y;
            vertices[v].z = z + 1;

            vertices[v + 1].x = x;
            vertices[v + 1].y = y + 1;
            vertices[v + 1].z = z + 1;

            vertices[v + 2].x = x;
            vertices[v + 2].y = y;
            vertices[v + 2].z = z;

            vertices[v + 3].x = x;
            vertices[v + 3].y = y + 1;
            vertices[v + 3].z = z;

            GenerateUvs(uvs, type, Left, v);

            v += 4;
            i += 4;

            addIndices(mesh.indices(), i);
        }
        if (mask & Right) {
            vertices[v].x = x + 1;
            vertices[v].y = y;
            vertices[v].z = z;

            vertices[v + 1].x = x + 1;
            vertices[v + 1].y = y + 1;
            vertices[v + 1].z = z;

            vertices[v + 2].x = x + 1;
            vertices[v + 2].y = y;
            vertices[v + 2].z = z + 1;

            vertices[v + 3].x = x + 1;
            vertices[v + 3].y = y + 1;
            vertices[v + 3].z = z + 1;

            GenerateUvs(uvs, type, Right, v);

            v += 4;
            i += 4;

            addIndices(mesh.indices(), i);
        }
        if (mask & Back) {
            vertices[v].x = x;
            vertices[v].y = y;
            vertices[v].z = z;

            vertices[v + 1].x = x;
            vertices[v + 1].y = y + 1;
            vertices[v + 1].z = z;

            vertices[v + 2].x = x + 1;
            vertices[v + 2].y = y;
            vertices[v + 2].z = z;

            vertices[v + 3].x = x + 1;
            vertices[v + 3].y = y + 1;
            vertices[v + 3].z = z;

            GenerateUvs(uvs, type, Back, v);

            v += 4;
            i += 4;

            addIndices(mesh.indices(), i);
        }
        if (mask & Front) {
            vertices[v].x = x + 1;
            vertices[v].y = y;
            vertices[v].z = z + 1;

            vertices[v + 1].x = x + 1;
            vertices[v + 1].y = y + 1;
            vertices[v + 1].z = z + 1;

            vertices[v + 2].x = x;
            vertices[v + 2].y = y;
            vertices[v + 2].z = z + 1;

            vertices[v + 3].x = x;
            vertices[v + 3].y = y + 1;
            vertices[v + 3].z = z + 1;

            GenerateUvs(uvs, type, Front, v);

            v += 4;
            i += 4;

            addIndices(mesh.indices(), i);
        }

        mesh.vertices().insert(mesh.vertices().end(), vertices.begin(), vertices.end());
        mesh.uv0().insert(mesh.uv0().end(), uvs.begin(), uvs.end());
        mesh.colors().insert(mesh.colors().end(), colors.begin(), colors.end());
    }

    virtual void GenerateUvs(std::vector<Vector2>& uvs, BlockType type, Sides side, uint32_t v) {
        int x0 = 0;
        int y0 = 15;
        switch (type) {
        case BlockType::Stone: x0 = 1; y0 = 15; break;
        case BlockType::Dirt: x0 = 2; y0 = 15; break;
        case BlockType::Bedrock: x0 = 1; y0 = 14; break;
        case BlockType::GoldOre: x0 = 0; y0 = 13; break;
        case BlockType::IronOre: x0 = 1; y0 = 13; break;
        case BlockType::CoalOre: x0 = 1; y0 = 13; break;
        case BlockType::Leaves: x0 = 4; y0 = 12; break;
        default: break;
        }

        float x1 = x0 * tileWidth;
        float x2 = (x0 + 1) * tileWidth;

        float y1 = y0 * tileHeight;
        float y2 = (y0 + 1) * tileHeight;

        uvs[v].x = x1;
        uvs[v].y = y1;

        uvs[v + 1].x = x1;
        uvs[v + 1].y = y2;

        uvs[v + 2].x = x2;
        uvs[v + 2].y = y1;

        uvs[v + 3].x = x2;
        uvs[v + 3].y = y2;
    }

    inline void addIndices(std::vector<uint32_t>& indices, uint32_t i) {
         indices.insert(indices.end(), {i - 4, i - 3, i - 2, i - 3, i - 1, i - 2});
    }
};
