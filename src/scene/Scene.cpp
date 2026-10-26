#include "scene/Scene.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string_view>
#include <unordered_set>
#include <utility>

namespace forge::scene {
namespace {

void appendUtf8(std::string& output, unsigned int codepoint) {
    if (codepoint <= 0x7F) {
        output.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) {
        output.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        output.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else {
        output.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }
}

void writeJsonString(std::ostream& output, std::string_view value) {
    static constexpr char hex[] = "0123456789abcdef";
    output.put('"');
    for (const unsigned char character : value) {
        switch (character) {
            case '"': output << "\\\""; break;
            case '\\': output << "\\\\"; break;
            case '\b': output << "\\b"; break;
            case '\f': output << "\\f"; break;
            case '\n': output << "\\n"; break;
            case '\r': output << "\\r"; break;
            case '\t': output << "\\t"; break;
            default:
                if (character < 0x20) {
                    output << "\\u00" << hex[character >> 4] << hex[character & 0x0F];
                } else {
                    output.put(static_cast<char>(character));
                }
        }
    }
    output.put('"');
}

class SceneReader {
public:
    explicit SceneReader(std::string_view input) : input_(input) {}

    bool read(std::vector<Entity>& entities, int& nextId, std::string& error) {
        skipWhitespace();
        if (!consume('{')) return fail(error, "Expected a scene JSON object");

        bool hasVersion = false;
        bool hasEntities = false;
        int version = 0;
        std::vector<Entity> parsedEntities;
        std::unordered_set<int> ids;
        skipWhitespace();
        while (!consume('}')) {
            std::string property;
            if (!readString(property, error) || !expect(':', error)) return false;
            if (property == "sceneVersion") {
                if (hasVersion) return fail(error, "Duplicate sceneVersion property");
                double number = 0.0;
                if (!readNumber(number, error) || std::floor(number) != number || number != 2.0) {
                    return fail(error, "Unsupported or invalid sceneVersion");
                }
                version = static_cast<int>(number);
                hasVersion = true;
            } else if (property == "entities") {
                if (hasEntities) return fail(error, "Duplicate entities property");
                if (!readEntities(parsedEntities, ids, error)) return false;
                hasEntities = true;
            } else if (!skipValue(0, error)) {
                return false;
            }

            skipWhitespace();
            if (consume('}')) break;
            if (!consume(',')) return fail(error, "Expected ',' or '}' in scene object");
            skipWhitespace();
        }
        skipWhitespace();
        if (position_ != input_.size()) return fail(error, "Unexpected data after scene object");
        if (!hasVersion || !hasEntities || version != 2) {
            return fail(error, "Scene requires a supported version and an entities array");
        }

        int candidateNextId = 1;
        for (const auto& entity : parsedEntities) {
            if (entity.id == std::numeric_limits<int>::max()) {
                return fail(error, "Entity id is too large");
            }
            candidateNextId = std::max(candidateNextId, entity.id + 1);
        }
        entities = std::move(parsedEntities);
        nextId = candidateNextId;
        return true;
    }

private:
    std::string_view input_;
    std::size_t position_{0};

    void skipWhitespace() {
        while (position_ < input_.size() &&
               (input_[position_] == ' ' || input_[position_] == '\t' ||
                input_[position_] == '\r' || input_[position_] == '\n')) {
            ++position_;
        }
    }

    bool consume(char expected) {
        skipWhitespace();
        if (position_ >= input_.size() || input_[position_] != expected) return false;
        ++position_;
        return true;
    }

    bool expect(char expected, std::string& error) {
        if (consume(expected)) return true;
        return fail(error, std::string("Expected '") + expected + "'");
    }

    bool fail(std::string& error, const std::string& message) const {
        error = message + " at byte " + std::to_string(position_);
        return false;
    }

    bool readHexQuad(unsigned int& value, std::string& error) {
        if (input_.size() - position_ < 4) return fail(error, "Incomplete Unicode escape");
        value = 0;
        for (int i = 0; i < 4; ++i) {
            const char digit = input_[position_++];
            value <<= 4;
            if (digit >= '0' && digit <= '9') value |= static_cast<unsigned int>(digit - '0');
            else if (digit >= 'a' && digit <= 'f') value |= static_cast<unsigned int>(digit - 'a' + 10);
            else if (digit >= 'A' && digit <= 'F') value |= static_cast<unsigned int>(digit - 'A' + 10);
            else return fail(error, "Invalid Unicode escape");
        }
        return true;
    }

    bool readString(std::string& output, std::string& error) {
        if (!consume('"')) return fail(error, "Expected a JSON string");
        output.clear();
        while (position_ < input_.size()) {
            const unsigned char character = static_cast<unsigned char>(input_[position_++]);
            if (character == '"') return true;
            if (character < 0x20) return fail(error, "Unescaped control character in string");
            if (character != '\\') {
                output.push_back(static_cast<char>(character));
                continue;
            }
            if (position_ >= input_.size()) return fail(error, "Incomplete string escape");
            switch (input_[position_++]) {
                case '"': output.push_back('"'); break;
                case '\\': output.push_back('\\'); break;
                case '/': output.push_back('/'); break;
                case 'b': output.push_back('\b'); break;
                case 'f': output.push_back('\f'); break;
                case 'n': output.push_back('\n'); break;
                case 'r': output.push_back('\r'); break;
                case 't': output.push_back('\t'); break;
                case 'u': {
                    unsigned int codepoint = 0;
                    if (!readHexQuad(codepoint, error)) return false;
                    if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
                        if (input_.size() - position_ < 2 ||
                            input_[position_] != '\\' || input_[position_ + 1] != 'u') {
                            return fail(error, "Missing low surrogate in Unicode escape");
                        }
                        position_ += 2;
                        unsigned int low = 0;
                        if (!readHexQuad(low, error) || low < 0xDC00 || low > 0xDFFF) {
                            return fail(error, "Invalid low surrogate in Unicode escape");
                        }
                        codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
                    } else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
                        return fail(error, "Unexpected low surrogate in Unicode escape");
                    }
                    appendUtf8(output, codepoint);
                    break;
                }
                default: return fail(error, "Invalid string escape");
            }
        }
        return fail(error, "Unterminated JSON string");
    }

    bool readNumber(double& value, std::string& error) {
        skipWhitespace();
        const std::size_t start = position_;
        if (position_ < input_.size() && input_[position_] == '-') ++position_;
        if (position_ >= input_.size()) return fail(error, "Invalid JSON number");
        if (input_[position_] == '0') {
            ++position_;
            if (position_ < input_.size() && input_[position_] >= '0' && input_[position_] <= '9') {
                return fail(error, "Leading zero in JSON number");
            }
        } else {
            const std::size_t integerStart = position_;
            while (position_ < input_.size() && input_[position_] >= '0' && input_[position_] <= '9') {
                ++position_;
            }
            if (position_ == integerStart) return fail(error, "Invalid JSON number");
        }
        if (position_ < input_.size() && input_[position_] == '.') {
            ++position_;
            const std::size_t fractionStart = position_;
            while (position_ < input_.size() && input_[position_] >= '0' && input_[position_] <= '9') {
                ++position_;
            }
            if (position_ == fractionStart) return fail(error, "Invalid JSON fraction");
        }
        if (position_ < input_.size() && (input_[position_] == 'e' || input_[position_] == 'E')) {
            ++position_;
            if (position_ < input_.size() && (input_[position_] == '+' || input_[position_] == '-')) {
                ++position_;
            }
            const std::size_t exponentStart = position_;
            while (position_ < input_.size() && input_[position_] >= '0' && input_[position_] <= '9') {
                ++position_;
            }
            if (position_ == exponentStart) return fail(error, "Invalid JSON exponent");
        }
        const std::string number(input_.substr(start, position_ - start));
        errno = 0;
        char* end = nullptr;
        value = std::strtod(number.c_str(), &end);
        if (errno == ERANGE || end != number.c_str() + number.size() || !std::isfinite(value)) {
            return fail(error, "JSON number is outside the supported range");
        }
        return true;
    }

    bool readBoolean(bool& value, std::string& error) {
        skipWhitespace();
        if (input_.substr(position_, 4) == "true") {
            position_ += 4;
            value = true;
            return true;
        }
        if (input_.substr(position_, 5) == "false") {
            position_ += 5;
            value = false;
            return true;
        }
        return fail(error, "Expected a JSON boolean");
    }

    bool readVector(Vec3& value, std::string& error) {
        if (!consume('[')) return fail(error, "Expected a three-number array");
        double components[3]{};
        for (int i = 0; i < 3; ++i) {
            if (!readNumber(components[i], error)) return false;
            if (i < 2 && !expect(',', error)) return false;
        }
        if (!expect(']', error)) return false;
        for (const double component : components) {
            if (component < -std::numeric_limits<float>::max() ||
                component > std::numeric_limits<float>::max()) {
                return fail(error, "Transform component is outside the supported range");
            }
        }
        value = {static_cast<float>(components[0]), static_cast<float>(components[1]),
                 static_cast<float>(components[2])};
        return true;
    }

    bool readEntity(Entity& entity, std::string& error) {
        if (!consume('{')) return fail(error, "Expected an entity object");
        bool hasId = false;
        bool hasName = false;
        bool hasType = false;
        std::unordered_set<std::string> properties;
        skipWhitespace();
        while (!consume('}')) {
            std::string property;
            if (!readString(property, error) || !expect(':', error)) return false;
            if (!properties.insert(property).second) {
                return fail(error, "Duplicate entity property: " + property);
            }
            if (property == "id") {
                double number = 0.0;
                if (!readNumber(number, error) || std::floor(number) != number ||
                    number < 1.0 || number >= static_cast<double>(std::numeric_limits<int>::max())) {
                    return fail(error, "Entity id must be a positive integer");
                }
                entity.id = static_cast<int>(number);
                hasId = true;
            } else if (property == "name") {
                if (!readString(entity.name, error)) return false;
                hasName = true;
            } else if (property == "type") {
                if (!readString(entity.type, error)) return false;
                hasType = true;
            } else if (property == "position") {
                if (!readVector(entity.position, error)) return false;
            } else if (property == "rotation") {
                if (!readVector(entity.rotation, error)) return false;
            } else if (property == "scale") {
                if (!readVector(entity.scale, error)) return false;
            } else if (property == "visible") {
                if (!readBoolean(entity.visible, error)) return false;
            } else if (property == "locked") {
                if (!readBoolean(entity.locked, error)) return false;
            } else if (property == "selected") {
                if (!readBoolean(entity.selected, error)) return false;
            } else if (!skipValue(0, error)) {
                return false;
            }

            skipWhitespace();
            if (consume('}')) break;
            if (!consume(',')) return fail(error, "Expected ',' or '}' in entity object");
            skipWhitespace();
        }
        if (!hasId || !hasName || !hasType || entity.name.empty() || entity.type.empty()) {
            return fail(error, "Entity requires a positive id, name, and type");
        }
        return true;
    }

    bool readEntities(std::vector<Entity>& entities, std::unordered_set<int>& ids,
                      std::string& error) {
        if (!consume('[')) return fail(error, "Expected an entities array");
        skipWhitespace();
        if (consume(']')) return true;
        bool hasSelection = false;
        while (true) {
            Entity entity;
            if (!readEntity(entity, error)) return false;
            if (!ids.insert(entity.id).second) return fail(error, "Duplicate entity id");
            if (entity.selected && hasSelection) return fail(error, "Scene has multiple selected entities");
            hasSelection = hasSelection || entity.selected;
            entities.push_back(std::move(entity));
            skipWhitespace();
            if (consume(']')) return true;
            if (!consume(',')) return fail(error, "Expected ',' or ']' in entities array");
        }
    }

    bool skipValue(unsigned int depth, std::string& error) {
        if (depth > 64) return fail(error, "JSON nesting limit exceeded");
        skipWhitespace();
        if (position_ >= input_.size()) return fail(error, "Expected a JSON value");
        if (input_[position_] == '"') {
            std::string ignored;
            return readString(ignored, error);
        }
        if (input_[position_] == '{') {
            ++position_;
            skipWhitespace();
            if (consume('}')) return true;
            while (true) {
                std::string ignored;
                if (!readString(ignored, error) || !expect(':', error) ||
                    !skipValue(depth + 1, error)) return false;
                if (consume('}')) return true;
                if (!expect(',', error)) return false;
            }
        }
        if (input_[position_] == '[') {
            ++position_;
            skipWhitespace();
            if (consume(']')) return true;
            while (true) {
                if (!skipValue(depth + 1, error)) return false;
                if (consume(']')) return true;
                if (!expect(',', error)) return false;
            }
        }
        if (input_.substr(position_, 4) == "true" || input_.substr(position_, 5) == "false") {
            bool ignored = false;
            return readBoolean(ignored, error);
        }
        if (input_.substr(position_, 4) == "null") {
            position_ += 4;
            return true;
        }
        double ignored = 0.0;
        return readNumber(ignored, error);
    }
};

} // namespace

void Scene::clear() {
    entities_.clear();
    nextId_ = 1;
}

Entity& Scene::add(const std::string& name, const std::string& type) {
    entities_.push_back({nextId_++, name, type});
    return entities_.back();
}

Entity& Scene::addCube() {
    return add("Cube", "Mesh");
}

void Scene::select(int id) {
    for (auto& entity : entities_) entity.selected = entity.id == id;
}

Entity* Scene::selected() {
    for (auto& entity : entities_) if (entity.selected) return &entity;
    return nullptr;
}

const Entity* Scene::selected() const {
    for (const auto& entity : entities_) if (entity.selected) return &entity;
    return nullptr;
}

void Scene::removeSelected() {
    const auto firstRemoved = std::remove_if(entities_.begin(), entities_.end(),
        [](const Entity& entity) { return entity.selected && !entity.locked; });
    entities_.erase(firstRemoved, entities_.end());
}

bool Scene::save(const std::filesystem::path& file, std::string& error) const {
    error.clear();
    std::unordered_set<int> ids;
    for (const auto& entity : entities_) {
        const auto finite = [](const Vec3& value) {
            return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
        };
        if (entity.id <= 0 || entity.id == std::numeric_limits<int>::max() ||
            !ids.insert(entity.id).second ||
            entity.name.empty() || entity.type.empty() ||
            !finite(entity.position) || !finite(entity.rotation) || !finite(entity.scale)) {
            error = "Scene contains an invalid entity or transform";
            return false;
        }
    }

    try {
        std::ofstream output(file, std::ios::trunc);
        if (!output) {
            error = "Cannot write scene file: " + file.string();
            return false;
        }
        output << std::setprecision(std::numeric_limits<float>::max_digits10);
        output << "{\n  \"sceneVersion\": 2,\n  \"entities\": [\n";
        for (std::size_t i = 0; i < entities_.size(); ++i) {
            const auto& entity = entities_[i];
            output << "    {\"id\":" << entity.id << ",\"name\":";
            writeJsonString(output, entity.name);
            output << ",\"type\":";
            writeJsonString(output, entity.type);
            output << ",\"position\":[" << entity.position.x << ',' << entity.position.y << ','
                   << entity.position.z << "],\"rotation\":[" << entity.rotation.x << ','
                   << entity.rotation.y << ',' << entity.rotation.z << "],\"scale\":["
                   << entity.scale.x << ',' << entity.scale.y << ',' << entity.scale.z
                   << "],\"visible\":" << (entity.visible ? "true" : "false")
                   << ",\"locked\":" << (entity.locked ? "true" : "false") << '}'
                   << (i + 1 < entities_.size() ? "," : "") << '\n';
        }
        output << "  ]\n}\n";
        if (!output) {
            error = "Failed while writing scene file: " + file.string();
            return false;
        }
        return true;
    } catch (const std::exception& exception) {
        error = exception.what();
        return false;
    }
}

bool Scene::load(const std::filesystem::path& file, std::string& error) {
    error.clear();
    try {
        std::ifstream input(file, std::ios::binary);
        if (!input) {
            error = "Scene file not found or unreadable: " + file.string();
            return false;
        }
        std::ostringstream contents;
        contents << input.rdbuf();
        if (!input.good() && !input.eof()) {
            error = "Failed while reading scene file: " + file.string();
            return false;
        }

        std::vector<Entity> parsedEntities;
        int parsedNextId = 1;
        if (!SceneReader(contents.str()).read(parsedEntities, parsedNextId, error)) return false;
        entities_ = std::move(parsedEntities);
        nextId_ = parsedNextId;
        return true;
    } catch (const std::exception& exception) {
        error = exception.what();
        return false;
    }
}

} // namespace forge::scene
