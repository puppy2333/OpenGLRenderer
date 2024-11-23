#include <string>
#include <iostream>
#include <fstream>

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

ModelData load_json()
{
    std::ifstream f("../settings/medieval_town_1.json");
    json data = json::parse(f);

    if (data.contains("model")) {
        ModelData model_data;

        model_data.gltf_path = data["model"]["gltf_path"];

        model_data.translate.x = data["model"]["translate"]["x"];
        model_data.translate.y = data["model"]["translate"]["y"];
        model_data.translate.z = data["model"]["translate"]["z"];

        model_data.scale.x = data["model"]["scale"]["x"];
        model_data.scale.y = data["model"]["scale"]["y"];
        model_data.scale.z = data["model"]["scale"]["z"];

        std::cout << "Loaded gltf path: " << model_data.gltf_path;

        return model_data;
    }
}