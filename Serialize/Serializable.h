#pragma once

#include<Serialize/Common/SerializeAPI.h>
#include<Serialize/Frontend/SerializeContext.h>
#include<Serialize/Frontend/DeserializeContext.h>

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
        bool SerializeToFile(const std::filesystem::path&, Pitaya::Serialize::API api = Pitaya::Serialize::API::YAML) const;
        bool DeserializeFromFile(const std::filesystem::path&, Pitaya::Serialize::API = Pitaya::Serialize::API::YAML);
        bool SerializeToString(std::string& ,Pitaya::Serialize::API = Pitaya::Serialize::API::YAML) const;
        bool DeserializeFromString(const std::string&, Pitaya::Serialize::API = Pitaya::Serialize::API::YAML);
    };
}
