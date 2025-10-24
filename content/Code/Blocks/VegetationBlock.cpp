#pragma once

#include "SolidBlock.cpp"

class VegetationBlock : public SolidBlock {
public:
    bool isCollidable() const override {
        return false;
    }

    void buildGeometry(Mesh &mesh, BlockType type, int8_t mask, int32_t x, int32_t y, int32_t z) override {
        Vector3Vector vertices;
        vertices.resize(8);

        Vector2Vector uvs;
        uvs.resize(8);
        
        Vector4Vector colors = Vector4Vector(8, Vector4(1.0f));

        uint32_t v = 0;
        uint32_t i = mesh.vertices().size();

        {
            vertices[v].x = x + 0.5f;
            vertices[v].y = y;
            vertices[v].z = z + 1;

            vertices[v + 1].x = x + 0.5f;
            vertices[v + 1].y = y + 1;
            vertices[v + 1].z = z + 1;

            vertices[v + 2].x = x + 0.5f;
            vertices[v + 2].y = y;
            vertices[v + 2].z = z;

            vertices[v + 3].x = x + 0.5f;
            vertices[v + 3].y = y + 1;
            vertices[v + 3].z = z;

            GenerateUvs(uvs, type, Left, v);

            v += 4;
            i += 4;

            addIndices(mesh.indices(), i);
        }
        {
            vertices[v].x = x;
            vertices[v].y = y;
            vertices[v].z = z + 0.5f;

            vertices[v + 1].x = x;
            vertices[v + 1].y = y + 1;
            vertices[v + 1].z = z + 0.5f;

            vertices[v + 2].x = x + 1;
            vertices[v + 2].y = y;
            vertices[v + 2].z = z + 0.5f;

            vertices[v + 3].x = x + 1;
            vertices[v + 3].y = y + 1;
            vertices[v + 3].z = z + 0.5f;

            GenerateUvs(uvs, type, Back, v);

            v += 4;
            i += 4;

            addIndices(mesh.indices(), i);
        }

        mesh.vertices().insert(mesh.vertices().end(), vertices.begin(), vertices.end());
        mesh.uv0().insert(mesh.uv0().end(), uvs.begin(), uvs.end());
        mesh.colors().insert(mesh.colors().end(), colors.begin(), colors.end());
    }

    void GenerateUvs(std::vector<Vector2>& uvs, BlockType type, Sides side, uint32_t v) override {
        int x0 = 0;
        int y0 = 15;

        switch (type) {
        case BlockType::TallGrass: {
            x0 = 7;
            y0 = 13;
            break;
        }
        case BlockType::Sapling: {
            x0 = 15;
            break;
        }
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
};
