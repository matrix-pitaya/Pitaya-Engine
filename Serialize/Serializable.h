#pragma once

#include<Serialize/Common/SerializeAPI.h>
#include<Serialize/Frontend/SerializeContext.h>
#include<Serialize/Frontend/DeserializeContext.h>

#include<Engine/API/def.h>

namespace Pitaya::Serialize
{
    class Serializable
    {
    public:
        Serializable() = default;
        virtual ~Serializable() = default;

    public:
        virtual void Serialize(SerializeContext&) const = 0;
        virtual void Deserialize(const DeserializeContext&) = 0;

    public:
        ENGINE_API bool ENGINE_CALL SerializeToFile(const std::filesystem::path&, Pitaya::Serialize::API api = Pitaya::Serialize::API::YAML) const;
        ENGINE_API bool ENGINE_CALL DeserializeFromFile(const std::filesystem::path&, Pitaya::Serialize::API = Pitaya::Serialize::API::YAML);
        ENGINE_API bool ENGINE_CALL SerializeToString(std::string& ,Pitaya::Serialize::API = Pitaya::Serialize::API::YAML) const;
        ENGINE_API bool ENGINE_CALL DeserializeFromString(const std::string&, Pitaya::Serialize::API = Pitaya::Serialize::API::YAML);
    };
}
