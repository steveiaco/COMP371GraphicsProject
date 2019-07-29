//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <list>

// A billboard has a position and a size in world units, and a texture
struct Billboard
{
    glm::vec3 position;
    float angle;
    glm::vec2 size;
    glm::vec4 color;
};

struct CompareBillboardAlongZ{
    bool operator()(const Billboard *a, const Billboard *b);
};


// We should render billboards in the fewest amount of render calls possible
// Billboards are semi-transparent, so they need to be sorted and rendered from back to front
class BillboardList
{
public:
    BillboardList(unsigned int maxNumBillboards, int textureID);
    ~BillboardList();
    
    void AddBillboard(Billboard* b);
    void RemoveBillboard(Billboard* b);
    
    void Update(float dt);
    void Draw();
    
private:
    // Each vertex on a billboard
    struct BillboardVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 color;
        glm::vec2 textureCoordinate;
    };

    
    std::vector<BillboardVertex> mVertexBuffer;
    std::list<Billboard*> mBillboardList;
    
    int mTextureID;
    unsigned int mMaxNumBillboards;

    unsigned int mVAO;
    unsigned int mVBO;
};
