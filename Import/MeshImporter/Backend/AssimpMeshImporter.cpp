#include<Import/MeshImporter/Backend/AssimpMeshImporter.h>
#include<Import/Common/MeshVertex.h>
#include<Asset/Common/Material.h>
#include<Asset/Common/Shader.h>
#include<Asset/Common/Texture.h>
#include<Asset/Common/FuncTable.h>
#include<Log/Common/FuncTable.h>

#include<Assimp/Importer.hpp>
#include<Assimp/Scene.h>
#include<Assimp/Postprocess.h>

#include<Core/Asset/Asset.h>

#include<glm.hpp>
#include<gtc/type_ptr.hpp>

bool Pitaya::Import::AssimpMeshImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, Pitaya::Import::MeshPreloadResult& out)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file.string(),
        aiProcess_Triangulate |
        aiProcess_ValidateDataStructure |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_GenUVCoords |
        aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0)
    {
        Pitaya::Log::Error("assimp scene is nor root node or error, GUID:" + guid.ToString() + " , path:" + file.string());
        out.IsValid = false;
        return false;
    }

    std::vector<Pitaya::Import::MeshPreloadResult::MeshNodeInfo> meshNodeInfos;
    TraverseNodes(scene->mRootNode, glm::mat4(1.0f), meshNodeInfos);

    if (meshNodeInfos.empty())
    {
        Pitaya::Log::Error("traverse mesh node info si empty, GUID:" + guid.ToString() + " , path:" + file.string());
        out.IsValid = false;
        return false;
    }

    out.SubMeshCount = static_cast<uint32_t>(meshNodeInfos.size());
    out.SubMeshPreloadResults.reserve(out.SubMeshCount);
    out.MeshNodeInfos.reserve(out.SubMeshCount);
    out.HasBones = false;
    out.TotalVertexCount = 0;
    out.TotalIndexCount = 0;

    //处理Mesh自带材质
    out.MaterialGUIDs.reserve(scene->mNumMaterials);
    for (uint32_t matIdx = 0; matIdx < scene->mNumMaterials; ++matIdx)
    {
        const aiMaterial* aiMat = scene->mMaterials[matIdx];
        aiString aiMatName;
        aiMat->Get(AI_MATKEY_NAME, aiMatName);
        std::string matName = aiMatName.C_Str();
        if (matName == AI_DEFAULT_MATERIAL_NAME || matName.empty())
        {
            //默认材质则使用引擎内置材质
            out.MaterialGUIDs.push_back(Pitaya::Asset::Material::Default);
            continue;
        }

        std::filesystem::path matFilePath = file.parent_path() / std::filesystem::path(matName + ".mat");
        std::filesystem::path virtualpath;
        if (!Pitaya::Asset::TransformToVirtualPath(matFilePath, file.parent_path(), virtualpath)) { continue; }
        Pitaya::Core::GUID matGuid;
        if (!Pitaya::Asset::GetAssetGUIDByPath(virtualpath, matGuid)) //材质不存在
        {
            //解析材质(内部会生成.mat文件)
            if (!ParseMaterial(aiMat, matFilePath, file))
            {
                Pitaya::Log::Error("failed to serialize material: " + matGuid.ToString());
                out.IsValid = false;
                return false;
            }
            Pitaya::Asset::RegisterExternalFile(matFilePath, file.parent_path(), virtualpath, matGuid);
        }
        out.MaterialGUIDs.push_back(matGuid);
    }

    for (uint32_t i = 0; i < out.SubMeshCount; i++)
    {
        const auto& currentNodeInfo = meshNodeInfos[i];
        const aiMesh* currentAiMesh = scene->mMeshes[currentNodeInfo.MeshIndex];
        Pitaya::Import::MeshPreloadResult::SubMeshPreloadResult currentSubPreload;

        currentSubPreload.VertexCount = currentAiMesh->mNumVertices;
        currentSubPreload.IndexCount = currentAiMesh->mNumFaces * 3;
        currentSubPreload.MaterialIndex = currentAiMesh->mMaterialIndex;
        currentSubPreload.HasNormals = currentAiMesh->HasNormals();
        currentSubPreload.HasUVs = currentAiMesh->HasTextureCoords(0);
        currentSubPreload.HasTangents = currentAiMesh->HasTangentsAndBitangents();
        currentSubPreload.HasBones = currentAiMesh->HasBones();

        glm::vec3 boundingMin = glm::vec3(FLT_MAX);
        glm::vec3 boundingMax = glm::vec3(-FLT_MAX);
        for (uint32_t v = 0; v < currentAiMesh->mNumVertices; v++)
        {
            const aiVector3D& vertexPos = currentAiMesh->mVertices[v];
            glm::vec4 localPos = glm::vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0f);
            glm::vec4 worldPos = currentNodeInfo.GlobalTransform * localPos;
            boundingMin.x = std::min(boundingMin.x, worldPos.x);
            boundingMin.y = std::min(boundingMin.y, worldPos.y);
            boundingMin.z = std::min(boundingMin.z, worldPos.z);
            boundingMax.x = std::max(boundingMax.x, worldPos.x);
            boundingMax.y = std::max(boundingMax.y, worldPos.y);
            boundingMax.z = std::max(boundingMax.z, worldPos.z);
        }
        currentSubPreload.BoundingBox.Min = boundingMin;
        currentSubPreload.BoundingBox.Max = boundingMax;

        out.MeshNodeInfos.push_back(currentNodeInfo);

        if (currentSubPreload.HasBones)
        {
            out.HasBones = true;
        }

        out.TotalVertexCount += currentSubPreload.VertexCount;
        out.TotalIndexCount += currentSubPreload.IndexCount;
        out.SubMeshPreloadResults.emplace_back(std::move(currentSubPreload));
    }

    out.Type = out.HasBones ? Pitaya::GPU::MeshType::SkinnedMesh : Pitaya::GPU::MeshType::Static;
    out.IsValid = true;
    return true;
}
bool Pitaya::Import::AssimpMeshImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& preload, Pitaya::Import::StaticMeshImportResult& out)
{
    // 必须保留 Flags: aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenSmoothNormals
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file.string(),
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Pitaya::Log::Error("Assimp Import Failed:" + std::string(importer.GetErrorString()));
        return false;
    }

    out.GUID = guid;
    out.SubMeshs.reserve(preload.SubMeshCount);

    // Shader 中统一写: layout(location=0) in vec3 Pos; ... layout(location=3) in vec4 Tan;
    out.VertexLayout = {
        { Pitaya::GPU::ShaderVariableType::Float3, 0 },     // Position
        { Pitaya::GPU::ShaderVariableType::Float3, 1 },     // Normal
        { Pitaya::GPU::ShaderVariableType::Float2, 2 },     // UV
        { Pitaya::GPU::ShaderVariableType::Float4, 3 }};    // Tangent

    // 计算包围盒初始值
    out.BoundingBox.Min = glm::vec3(FLT_MAX);
    out.BoundingBox.Max = glm::vec3(-FLT_MAX);

    uint32_t totalVertexCount = 0;
    uint32_t totalIndexCount = 0;
    for (const auto& sub : preload.SubMeshPreloadResults)
    {
        totalVertexCount += sub.VertexCount;
        totalIndexCount += sub.IndexCount;
    }

    // StaticMeshVertex 大小为 12 个 float
    const uint32_t strideFloats = sizeof(StaticMeshVertex) / sizeof(float);

    out.Vertices.resize(totalVertexCount * strideFloats);
    out.Indices.resize(totalIndexCount);

    // 获取强类型指针，方便操作
    StaticMeshVertex* vertexBufferPtr = reinterpret_cast<StaticMeshVertex*>(out.Vertices.data());
    uint32_t* indexBufferPtr = out.Indices.data();

    // 填充数据
    uint32_t globalVertexOffset = 0;
    uint32_t globalIndexOffset = 0;
    for (uint32_t i = 0; i < preload.SubMeshCount; i++)
    {
        const auto& currentSubPreload = preload.SubMeshPreloadResults[i];
        const auto& currentNodeInfo = preload.MeshNodeInfos[i];

        // 安全检查：索引是否越界
        if (currentNodeInfo.MeshIndex >= scene->mNumMeshes) continue;
        const aiMesh* currentAiMesh = scene->mMeshes[currentNodeInfo.MeshIndex];

        // 填充 SubMesh 信息
        Pitaya::Asset::Mesh::SubMesh subInfo;
        subInfo.BaseVertex = globalVertexOffset;
        subInfo.BaseIndex = globalIndexOffset;
        subInfo.MaterialIndex = currentAiMesh->mMaterialIndex;
        subInfo.IndexCount = currentSubPreload.IndexCount;
        const glm::mat4& meshGlobalTransform = currentNodeInfo.GlobalTransform;
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(meshGlobalTransform)));
        for (uint32_t v = 0; v < currentSubPreload.VertexCount; v++)
        {
            // 当前顶点引用
            StaticMeshVertex& vert = vertexBufferPtr[globalVertexOffset + v];

            // Position
            const aiVector3D& aiPos = currentAiMesh->mVertices[v];
            glm::vec4 localPos(aiPos.x, aiPos.y, aiPos.z, 1.0f);
            vert.Position = glm::vec3(meshGlobalTransform * localPos);

            // 更新包围盒
            out.BoundingBox.Min = glm::min(out.BoundingBox.Min, vert.Position);
            out.BoundingBox.Max = glm::max(out.BoundingBox.Max, vert.Position);

            // Normal (若无，默认指向上方)
            if (currentAiMesh->HasNormals())
            {
                const aiVector3D& aiNorm = currentAiMesh->mNormals[v];
                // 变换并归一化
                vert.Normal = glm::normalize(normalMatrix * glm::vec3(aiNorm.x, aiNorm.y, aiNorm.z));
            }
            else
            {
                vert.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            // UV (只取第一层 UV，若无则填 0)
            if (currentAiMesh->HasTextureCoords(0))
            {
                vert.UV = glm::vec2(currentAiMesh->mTextureCoords[0][v].x, currentAiMesh->mTextureCoords[0][v].y);
            }
            else
            {
                vert.UV = glm::vec2(0.0f, 0.0f);
            }

            // Tangent (若无，默认填 0)
            if (currentAiMesh->HasTangentsAndBitangents())
            {
                const aiVector3D& aiTan = currentAiMesh->mTangents[v];
                const aiVector3D& aiBiTan = currentAiMesh->mBitangents[v];
                const aiVector3D& aiNorm = currentAiMesh->mNormals[v]; // 原始法线用于计算手性

                // 变换到世界空间
                glm::vec3 T = glm::normalize(normalMatrix * glm::vec3(aiTan.x, aiTan.y, aiTan.z));
                glm::vec3 B = glm::normalize(normalMatrix * glm::vec3(aiBiTan.x, aiBiTan.y, aiBiTan.z));
                glm::vec3 N = glm::normalize(normalMatrix * glm::vec3(aiNorm.x, aiNorm.y, aiNorm.z)); // 重新计算一次N以确保系一致

                // 计算 Handedness (W 分量)
                // Det(T, B, N) < 0 表示由左手系变成了右手系(或反之)，通常用于处理镜像 UV
                float handedness = (glm::dot(glm::cross(N, T), B) < 0.0f) ? -1.0f : 1.0f;

                vert.Tangent = glm::vec4(T, handedness);
            }
            else
            {
                vert.Tangent = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        for (uint32_t f = 0; f < currentAiMesh->mNumFaces; f++)
        {
            const aiFace& face = currentAiMesh->mFaces[f];
            for (uint32_t j = 0; j < face.mNumIndices; j++)
            {
                indexBufferPtr[globalIndexOffset + f * 3 + j] = face.mIndices[j];
            }
        }
        out.SubMeshs.emplace_back(std::move(subInfo));

        globalVertexOffset += currentSubPreload.VertexCount;
        globalIndexOffset += currentSubPreload.IndexCount;
    }

    out.MaterialGUIDs = preload.MaterialGUIDs;
    out.IsValid = true;
    return true;
}
bool Pitaya::Import::AssimpMeshImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const Pitaya::Import::MeshPreloadResult& preload, Pitaya::Import::SkinnedMeshImportResult& out)
{
    if (!preload.IsValid || preload.Type != Pitaya::GPU::MeshType::SkinnedMesh)
    {
        Pitaya::Log::Error("Mesh is invalid or type error, GUID:" + guid.ToString() + " , path:" + file.string());
        out.IsValid = false;
        return false;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file.string(),
        aiProcess_Triangulate |
        aiProcess_ValidateDataStructure |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_GenUVCoords |
        aiProcess_FlipUVs |
        aiProcess_LimitBoneWeights); // 限制每顶点最大骨骼数为4（通常 Assimp 默认就是4）

    if (!scene || !scene->mRootNode)
    {
        Pitaya::Log::Error("Assimp scene invalid or no root node, GUID:" + guid.ToString());
        out.IsValid = false;
        return false;
    }

    // 定义固定布局 (Fixed Layout) - Skinned
    out.GUID = guid;
    out.VertexLayout = {
        { Pitaya::GPU::ShaderVariableType::Float3, 0 },         // Position
        { Pitaya::GPU::ShaderVariableType::Float3, 1 },         // Normal
        { Pitaya::GPU::ShaderVariableType::Float2, 2 },         // UV
        { Pitaya::GPU::ShaderVariableType::Float4, 3 },         // Tangent
        { Pitaya::GPU::ShaderVariableType::Float4, 4, true },   // Weights (Normalized)
        { Pitaya::GPU::ShaderVariableType::Float4, 5 }};          // Indices (存储在float buffer中)
        // 注意：如果是 Int4，通常需要单独的整数 Buffer，但这里复用 Vertices(float vector)，
        // 所以 Layout 建议配合 Float4 使用，或者 Shader 侧做 float->int 转换。

    // 计算包围盒初始值
    out.BoundingBox.Min = glm::vec3(FLT_MAX);
    out.BoundingBox.Max = glm::vec3(-FLT_MAX);

    uint32_t totalVertexCount = 0;
    uint32_t totalIndexCount = 0;
    for (const auto& sub : preload.SubMeshPreloadResults)
    {
        totalVertexCount += sub.VertexCount;
        totalIndexCount += sub.IndexCount;
    }

    // SkinnedVertex 大小为 20 floats
    const uint32_t strideFloats = sizeof(SkinnedMeshVertex) / sizeof(float);
    out.Vertices.resize(totalVertexCount * strideFloats);
    out.Indices.resize(totalIndexCount);

    out.SubMeshs.reserve(preload.SubMeshCount);
    out.BoneInverseMatrices.reserve(100); // 预估骨骼数量

    // 获取指针方便操作
    SkinnedMeshVertex* vertexBufferPtr = reinterpret_cast<SkinnedMeshVertex*>(out.Vertices.data());
    uint32_t* indexBufferPtr = out.Indices.data();

    uint32_t globalVertexOffset = 0;
    uint32_t globalIndexOffset = 0;

    // 骨骼映射表 (Name -> Local ID)
    std::unordered_map<std::string, uint32_t> boneNameToIdMap;
    for (uint32_t i = 0; i < preload.SubMeshCount; i++)
    {
        const auto& currentSubPreload = preload.SubMeshPreloadResults[i];
        const auto& currentNodeInfo = preload.MeshNodeInfos[i];

        if (currentNodeInfo.MeshIndex >= scene->mNumMeshes) continue;
        const aiMesh* currentAiMesh = scene->mMeshes[currentNodeInfo.MeshIndex];

        // 填充 SubMesh 信息
        Pitaya::Asset::Mesh::SubMesh currentSubMesh;
        currentSubMesh.IndexCount = currentSubPreload.IndexCount;
        currentSubMesh.BaseIndex = globalIndexOffset;
        currentSubMesh.BaseVertex = globalVertexOffset;
        currentSubMesh.MaterialIndex = currentAiMesh->mMaterialIndex;
        // 先重置当前 SubMesh 包围盒
        currentSubMesh.BoundingBox.Min = glm::vec3(FLT_MAX);
        currentSubMesh.BoundingBox.Max = glm::vec3(-FLT_MAX);

        // 准备变换矩阵 (将 Mesh 变换烘焙进顶点)
        const glm::mat4& meshGlobalTransform = currentNodeInfo.GlobalTransform;
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(meshGlobalTransform)));

        // 填充基础几何数据
        for (uint32_t v = 0; v < currentSubPreload.VertexCount; v++)
        {
            SkinnedMeshVertex& vert = vertexBufferPtr[globalVertexOffset + v];

            // Position
            const aiVector3D& aiPos = currentAiMesh->mVertices[v];
            glm::vec4 localPos(aiPos.x, aiPos.y, aiPos.z, 1.0f);
            vert.Position = glm::vec3(meshGlobalTransform * localPos);

            // 更新包围盒 (SubMesh & Total)
            currentSubMesh.BoundingBox.Min = glm::min(currentSubMesh.BoundingBox.Min, vert.Position);
            currentSubMesh.BoundingBox.Max = glm::max(currentSubMesh.BoundingBox.Max, vert.Position);
            out.BoundingBox.Min = glm::min(out.BoundingBox.Min, vert.Position);
            out.BoundingBox.Max = glm::max(out.BoundingBox.Max, vert.Position);

            // Normal
            if (currentAiMesh->HasNormals())
            {
                const aiVector3D& aiNorm = currentAiMesh->mNormals[v];
                vert.Normal = glm::normalize(normalMatrix * glm::vec3(aiNorm.x, aiNorm.y, aiNorm.z));
            }
            else
            {
                vert.Normal = glm::vec3(0, 1, 0);
            }

            // UV
            if (currentAiMesh->HasTextureCoords(0))
            {
                vert.UV = glm::vec2(currentAiMesh->mTextureCoords[0][v].x, currentAiMesh->mTextureCoords[0][v].y);
            }
            else
            {
                vert.UV = glm::vec2(0, 0);
            }

            // Tangent
            if (currentAiMesh->HasTangentsAndBitangents())
            {
                glm::vec3 T = glm::normalize(normalMatrix * glm::vec3(currentAiMesh->mTangents[v].x, currentAiMesh->mTangents[v].y, currentAiMesh->mTangents[v].z));
                glm::vec3 B = glm::normalize(normalMatrix * glm::vec3(currentAiMesh->mBitangents[v].x, currentAiMesh->mBitangents[v].y, currentAiMesh->mBitangents[v].z));
                glm::vec3 N = glm::normalize(normalMatrix * glm::vec3(currentAiMesh->mNormals[v].x, currentAiMesh->mNormals[v].y, currentAiMesh->mNormals[v].z));

                float handedness = (glm::dot(glm::cross(N, T), B) < 0.0f) ? -1.0f : 1.0f;
                vert.Tangent = glm::vec4(T, handedness);
            }
            else
            {
                vert.Tangent = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }

            // 初始化权重 index 为 0
            vert.BoneWeights = glm::vec4(0.0f);
            vert.BoneIndices = glm::vec4(0.0f);
        }

        // 填充骨骼权重 (Weights & Indices)
        for (uint32_t b = 0; b < currentAiMesh->mNumBones; b++)
        {
            const aiBone* currentBone = currentAiMesh->mBones[b];
            std::string boneName(currentBone->mName.C_Str());
            uint32_t boneId = 0;

            // 注册/查找骨骼 ID
            if (boneNameToIdMap.find(boneName) == boneNameToIdMap.end())
            {
                boneId = static_cast<uint32_t>(out.BoneInverseMatrices.size());
                boneNameToIdMap[boneName] = boneId;

                // 转换 Offset Matrix (Assimp 格式 -> GLM)
                const aiMatrix4x4& aiOffset = currentBone->mOffsetMatrix;
                glm::mat4 offsetMat;
                offsetMat[0][0] = aiOffset.a1; offsetMat[0][1] = aiOffset.b1; offsetMat[0][2] = aiOffset.c1; offsetMat[0][3] = aiOffset.d1;
                offsetMat[1][0] = aiOffset.a2; offsetMat[1][1] = aiOffset.b2; offsetMat[1][2] = aiOffset.c2; offsetMat[1][3] = aiOffset.d2;
                offsetMat[2][0] = aiOffset.a3; offsetMat[2][1] = aiOffset.b3; offsetMat[2][2] = aiOffset.c3; offsetMat[2][3] = aiOffset.d3;
                offsetMat[3][0] = aiOffset.a4; offsetMat[3][1] = aiOffset.b4; offsetMat[3][2] = aiOffset.c4; offsetMat[3][3] = aiOffset.d4;

                // Assimp 矩阵是 Row-Major，GLM 是 Column-Major，通常 ReadFile 时已经是 Row-Major 内存布局，
                // 但 aiMatrix4x4 成员通常需要转置才能直接对应 glm::mat4 的内存构造。
                // 上面的逐个赋值实际上是在做转置操作 (aiOffset.a1 是 Row1Col1)。
                // 如果需要严格转置，请使用 glm::transpose。上面赋值是按行赋给GLM(列主序)，所以结果是转置后的。
                // 修正：aiMatrix4x4 访问成员就是 Row 1 Col 1。
                // GLM [col][row]。
                // 如果我们写 mat[0][0] = a1 ... 这是把行数据直接填入，等同于没有转置。
                // 此时 mat * vec 行为正确。
                out.BoneInverseMatrices.push_back(glm::transpose(offsetMat));
            }
            else
            {
                boneId = boneNameToIdMap[boneName];
            }

            // 填充受影响的顶点
            for (uint32_t w = 0; w < currentBone->mNumWeights; w++)
            {
                const aiVertexWeight& aiWeight = currentBone->mWeights[w];
                uint32_t vertexId = aiWeight.mVertexId;

                // 确保不越界
                if (vertexId >= currentSubPreload.VertexCount) continue;

                SkinnedMeshVertex& vert = vertexBufferPtr[globalVertexOffset + vertexId];

                // 找到第一个空闲的权重槽 (Weight == 0)
                if (vert.BoneWeights.x == 0.0f)
                {
                    vert.BoneWeights.x = aiWeight.mWeight;
                    vert.BoneIndices.x = static_cast<float>(boneId);
                }
                else if (vert.BoneWeights.y == 0.0f)
                {
                    vert.BoneWeights.y = aiWeight.mWeight;
                    vert.BoneIndices.y = static_cast<float>(boneId);
                }
                else if (vert.BoneWeights.z == 0.0f)
                {
                    vert.BoneWeights.z = aiWeight.mWeight;
                    vert.BoneIndices.z = static_cast<float>(boneId);
                }
                else if (vert.BoneWeights.w == 0.0f)
                {
                    vert.BoneWeights.w = aiWeight.mWeight;
                    vert.BoneIndices.w = static_cast<float>(boneId);
                }
                // 如果4个都满了 已经设置aiProcess_LimitBoneWeights 直接忽略多余的
            }
        }

        // 索引填充
        for (uint32_t f = 0; f < currentAiMesh->mNumFaces; f++)
        {
            const aiFace& face = currentAiMesh->mFaces[f];
            if (face.mNumIndices != 3) continue;

            indexBufferPtr[globalIndexOffset + f * 3 + 0] = face.mIndices[0];
            indexBufferPtr[globalIndexOffset + f * 3 + 1] = face.mIndices[1];
            indexBufferPtr[globalIndexOffset + f * 3 + 2] = face.mIndices[2];
        }

        // 权重归一化 确保每个顶点的权重和为 1.0
        for (uint32_t v = 0; v < currentSubPreload.VertexCount; v++)
        {
            SkinnedMeshVertex& vert = vertexBufferPtr[globalVertexOffset + v];
            float total = vert.BoneWeights.x + vert.BoneWeights.y + vert.BoneWeights.z + vert.BoneWeights.w;
            if (total > 0.0001f)
            {
                vert.BoneWeights /= total;
            }
        }

        out.SubMeshs.push_back(std::move(currentSubMesh));

        globalVertexOffset += currentSubPreload.VertexCount;
        globalIndexOffset += currentSubPreload.IndexCount;
    }

    out.BoneCount = static_cast<uint32_t>(out.BoneInverseMatrices.size());
    out.MaterialGUIDs = preload.MaterialGUIDs;
    out.IsValid = true;
    return true;
}

void Pitaya::Import::AssimpMeshImporter::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, std::vector<Pitaya::Import::MeshPreloadResult::MeshNodeInfo>& outMeshInfos)
{
    aiMatrix4x4 aiLocalTransform = node->mTransformation;
    glm::mat4 localTransform = glm::mat4(1.0f);
    localTransform[0][0] = aiLocalTransform.a1;
    localTransform[0][1] = aiLocalTransform.b1;
    localTransform[0][2] = aiLocalTransform.c1;
    localTransform[0][3] = aiLocalTransform.d1;
    localTransform[1][0] = aiLocalTransform.a2;
    localTransform[1][1] = aiLocalTransform.b2;
    localTransform[1][2] = aiLocalTransform.c2;
    localTransform[1][3] = aiLocalTransform.d2;
    localTransform[2][0] = aiLocalTransform.a3;
    localTransform[2][1] = aiLocalTransform.b3;
    localTransform[2][2] = aiLocalTransform.c3;
    localTransform[2][3] = aiLocalTransform.d3;
    localTransform[3][0] = aiLocalTransform.a4;
    localTransform[3][1] = aiLocalTransform.b4;
    localTransform[3][2] = aiLocalTransform.c4;
    localTransform[3][3] = aiLocalTransform.d4;

    glm::mat4 globalTransform = parentTransform * localTransform;
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        Pitaya::Import::MeshPreloadResult::MeshNodeInfo currentInfo;
        currentInfo.MeshIndex = node->mMeshes[i];
        currentInfo.GlobalTransform = globalTransform;
        outMeshInfos.push_back(currentInfo);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        TraverseNodes(node->mChildren[i], globalTransform, outMeshInfos);
    }
}
bool Pitaya::Import::AssimpMeshImporter::ParseMaterial(const aiMaterial* aimaterial, const std::filesystem::path& matFilePath, const std::filesystem::path& modelFilePath)
{
    Pitaya::Asset::Material material;

    //设置材质Shader为默认Shader
    Pitaya::Core::Asset<Pitaya::Asset::Shader>::AssetEntry dummyAssetEntry_SHADER;  //dunmmyentry 用于material临时序列化
    dummyAssetEntry_SHADER.GUID = Pitaya::Asset::Shader::Default;
    material.Shader = &dummyAssetEntry_SHADER;

    //设置材质属性
    material.Property.Variables.clear();
    for (unsigned int i = 0; i < aimaterial->mNumProperties; ++i)
    {
        const aiMaterialProperty* prop = aimaterial->mProperties[i];
        if (!prop) { continue; }

        std::string propKey = prop->mKey.C_Str();
        if (prop->mType == aiPTI_Float)
        {
            int numFloats = prop->mDataLength / sizeof(float);
            const float* dataPtr = reinterpret_cast<const float*>(prop->mData);
            if (numFloats == 1) { material.Property.Variables[propKey] = dataPtr[0]; }
            else if (numFloats == 2) { material.Property.Variables[propKey] = glm::vec2(dataPtr[0], dataPtr[1]); }
            else if (numFloats == 3) { material.Property.Variables[propKey] = glm::vec3(dataPtr[0], dataPtr[1], dataPtr[2]); }
            else if (numFloats == 4) { material.Property.Variables[propKey] = glm::vec4(dataPtr[0], dataPtr[1], dataPtr[2], dataPtr[3]); }
        }
        else if (prop->mType == aiPTI_Double)
        {
            if (prop->mDataLength >= sizeof(double))
            {
                double val = *reinterpret_cast<const double*>(prop->mData);
                material.Property.Variables[propKey] = static_cast<float>(val);
            }
        }
        else if (prop->mType == aiPTI_Integer)
        {
            if (prop->mDataLength >= sizeof(int))
            {
                int val = *reinterpret_cast<const int*>(prop->mData);
                material.Property.Variables[propKey] = val;
            }
        }
    }

    //设置材质纹理
    constexpr const aiTextureType texTypes[] = {
        aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_AMBIENT, aiTextureType_EMISSIVE,
        aiTextureType_HEIGHT, aiTextureType_NORMALS, aiTextureType_SHININESS, aiTextureType_OPACITY,
        aiTextureType_DISPLACEMENT, aiTextureType_LIGHTMAP, aiTextureType_REFLECTION,
        aiTextureType_BASE_COLOR, aiTextureType_METALNESS, aiTextureType_DIFFUSE_ROUGHNESS,
        aiTextureType_AMBIENT_OCCLUSION };

    //dunmmyentry 用于material临时序列化
    Pitaya::Core::Asset<Pitaya::Asset::Texture>::AssetEntry dummyAssetEntry_TEXTURES[Pitaya::GPU::MaterialTextureSlotCount] = {};
    for (auto aiType : texTypes)
    {
        aiString path;
        if (aimaterial->GetTexture(aiType, 0, &path) == AI_SUCCESS)
        {
            Pitaya::GPU::TextureSlot usage = AiTextureTypeToTextureUsage(aiType);
            if (static_cast<uint8_t>(usage) >= Pitaya::GPU::MaterialTextureSlotCount) { continue; }

            Pitaya::Core::GUID texGuid;
            std::filesystem::path virtualpath;
            if (!(Pitaya::Asset::TransformToVirtualPath(path.C_Str(), modelFilePath.parent_path(), virtualpath) &&
                Pitaya::Asset::GetAssetGUIDByPath(virtualpath, texGuid)))
            {
                //转换失败 使用默认白色纹理
                texGuid = Pitaya::Asset::Texture::White;
            }
            dummyAssetEntry_TEXTURES[static_cast<uint8_t>(usage)].GUID = texGuid;
            material.Textures[static_cast<uint8_t>(usage)] = &dummyAssetEntry_TEXTURES[static_cast<uint8_t>(usage)];
        }
    }

    return material.SerializeToFile(matFilePath);
}
