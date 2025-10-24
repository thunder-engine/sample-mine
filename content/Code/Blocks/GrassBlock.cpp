#pragma once

#include "SolidBlock.cpp"

class GrassBlock : public SolidBlock {
public:
    void GenerateUvs(std::vector<Vector2>& uvs, BlockType type, Sides side, uint32_t v) override {
        int x0 = 0;
        int y0 = 15;

        switch (type) {
        case BlockType::Grass: {
            x0 = 3;
            if (side == Top) {
                x0 = 0;
            }
            else if (side == Bottom) {
                x0 = 2;
            }
            y0 = 15;
            break;
        }
        case BlockType::Log: {
            x0 = 4;
            if (side == Top || side == Bottom) {
                x0 = 5;
            }
            y0 = 14;
            break;
        }
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
