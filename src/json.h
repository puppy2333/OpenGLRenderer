#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

using json = nlohmann::json;

class ModelData
{
public:
    std::string gltf_path;
    glm::vec3 translate;
    glm::vec3 scale;
};

std::shared_ptr<ModelData> json_load_model(std::string json_path = "../settings/medieval_town_1.json")
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    if (data.contains("model")) {
        std::shared_ptr<ModelData> model_data = std::make_shared<ModelData>();

        model_data->gltf_path = data["model"]["gltf_path"];

        model_data->translate.x = data["model"]["translate"]["x"];
        model_data->translate.y = data["model"]["translate"]["y"];
        model_data->translate.z = data["model"]["translate"]["z"];

        model_data->scale.x = data["model"]["scale"]["x"];
        model_data->scale.y = data["model"]["scale"]["y"];
        model_data->scale.z = data["model"]["scale"]["z"];

        std::cout << "Loaded gltf path: " << model_data->gltf_path;

        return model_data;
    }
    return nullptr;
}

class LightData
{
public:
    glm::vec3 position;
    glm::vec3 color;
};

std::shared_ptr<LightData> json_load_light(std::string json_path = "../settings/medieval_town_1.json")
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    if (data.contains("light")) {
        std::shared_ptr<LightData> light_data = std::make_shared<LightData>();

        light_data->position.x = data["light"]["position"]["x"];
        light_data->position.y = data["light"]["position"]["y"];
        light_data->position.z = data["light"]["position"]["z"];

        light_data->color.x = data["light"]["color"]["x"];
        light_data->color.y = data["light"]["color"]["y"];
        light_data->color.z = data["light"]["color"]["z"];

        return light_data;
    }
    return nullptr;
}