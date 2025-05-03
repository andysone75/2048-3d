#include "Engine.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Mesh Mesh::GenCube(Color color) {
    float hs = 0.5f;

    float vertices[] = {
        -hs, -hs,  hs,  hs, -hs,  hs,  hs,  hs,  hs,
         hs,  hs,  hs, -hs,  hs,  hs, -hs, -hs,  hs,
        -hs, -hs, -hs, -hs,  hs, -hs,  hs,  hs, -hs,
         hs,  hs, -hs,  hs, -hs, -hs, -hs, -hs, -hs,
        -hs, -hs, -hs, -hs, -hs,  hs, -hs,  hs,  hs,
        -hs,  hs,  hs, -hs,  hs, -hs, -hs, -hs, -hs,
         hs, -hs,  hs,  hs, -hs, -hs,  hs,  hs, -hs,
         hs,  hs, -hs,  hs,  hs,  hs,  hs, -hs,  hs,
        -hs,  hs,  hs,  hs,  hs,  hs,  hs,  hs, -hs,
         hs,  hs, -hs, -hs,  hs, -hs, -hs,  hs,  hs,
        -hs, -hs, -hs,  hs, -hs, -hs,  hs, -hs,  hs,
         hs, -hs,  hs, -hs, -hs,  hs, -hs, -hs, -hs
    };

    float normals[] = {
        0, 0,  1,  0,  0,  1,  0,  0,  1,
        0, 0,  1,  0,  0,  1,  0,  0,  1,
        0, 0, -1,  0,  0, -1,  0,  0, -1,
        0, 0, -1,  0,  0, -1,  0,  0, -1,
       -1, 0,  0, -1,  0,  0, -1,  0,  0,
       -1, 0,  0, -1,  0,  0, -1,  0,  0,
        1, 0,  0,  1,  0,  0,  1,  0,  0,
        1, 0,  0,  1,  0,  0,  1,  0,  0,
        0, 1,  0,  0,  1,  0,  0,  1,  0,
        0, 1,  0,  0,  1,  0,  0,  1,  0,
        0, -1, 0,  0, -1,  0,  0, -1,  0,
        0, -1, 0,  0, -1,  0,  0, -1,  0
    };

    unsigned char colors[36 * 4];
    for (int i = 0; i < 36; i++) {
        colors[i * 4 + 0] = color.r;
        colors[i * 4 + 1] = color.g;
        colors[i * 4 + 2] = color.b;
        colors[i * 4 + 3] = color.a;
    }

    float uvs[] = {
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f
    };

    Mesh mesh;
    mesh.vertexCount = 36;
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    GLuint vbo[4];
    glGenBuffers(4, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    return mesh;
}


Mesh GenPlane(float width, float height, int slices) {
    int vertexCount = (slices + 1) * (slices + 1);
    int indexCount = slices * slices * 6;

    std::vector<glm::vec3> vertices(vertexCount);
    std::vector<GLuint> indices(indexCount);

    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    float step = width / slices;

    int vertexIndex = 0;
    for (int z = 0; z <= slices; ++z) {
        for (int x = 0; x <= slices; ++x) {
            float xPos = -halfWidth + x * step;
            float zPos = -halfHeight + z * step;
            vertices[vertexIndex++] = glm::vec3(xPos, 0.0f, zPos);
        }
    }

    int indexIndex = 0;
    for (int z = 0; z < slices; ++z) {
        for (int x = 0; x < slices; ++x) {
            int topLeft = z * (slices + 1) + x;
            int topRight = z * (slices + 1) + (x + 1);
            int bottomLeft = (z + 1) * (slices + 1) + x;
            int bottomRight = (z + 1) * (slices + 1) + (x + 1);

            indices[indexIndex++] = topLeft;
            indices[indexIndex++] = bottomLeft;
            indices[indexIndex++] = topRight;

            indices[indexIndex++] = topRight;
            indices[indexIndex++] = bottomLeft;
            indices[indexIndex++] = bottomRight;
        }
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = VAO;
    mesh.vertexCount = vertexCount;

    return mesh;
}

Mesh Mesh::GenFullscreenQuad() {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // texCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    Mesh mesh;
    mesh.vao = quadVAO;
    mesh.vertexCount = 6;
    return mesh;
}

Model Model::Load(const Mesh& mesh) {
    Model model;
    model.mesh = mesh;
    return model;
}

std::string loadShaderSource(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

Shader Shader::Load(const std::string& vsPath, const std::string& fsPath) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, loadShaderSource(vsPath).c_str());
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, loadShaderSource(fsPath).c_str());
    GLuint id = glCreateProgram();

    // Check if all working without this commands
    //glBindAttribLocation(id, 0, "aPosition");
    //glBindAttribLocation(id, 1, "aNormal");
    //glBindAttribLocation(id, 2, "aColor");
    //glBindAttribLocation(id, 3, "aUv");

    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << '\n';
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return Shader{ id };
}