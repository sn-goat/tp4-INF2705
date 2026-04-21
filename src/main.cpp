#include <cstddef>
#include <cstdint>

#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <inf2705/OpenGLApplication.hpp>

#include "model.hpp"
#include "textures.hpp"
#include "shaders.hpp"

#define CHECK_GL_ERROR printGLError(__FILE__, __LINE__)

using namespace gl;
using namespace glm;


struct Transform
{
    vec3 position = vec3(0.0f);
    vec3 rotation = vec3(0.0f); // euler degrees
    float scale = 1.0f;

    mat4 matrix() const
    {
        mat4 m = translate(mat4(1.0f), position);
        m = rotate(m, radians(rotation.y), vec3(0, 1, 0));
        m = rotate(m, radians(rotation.x), vec3(1, 0, 0));
        m = rotate(m, radians(rotation.z), vec3(0, 0, 1));
        m = glm::scale(m, vec3(scale));
        return m;
    }
};


struct App : public OpenGLApplication
{
    App()
    : cameraPosition_(0.f, 1.5f, 5.f)
    , cameraOrientation_(0.f, 0.f)
    , isMouseMotionEnabled_(false)
    , totalTime_(0.0f)
    {
    }

    void init() override
    {
        setKeybindMessage(
            "ESC : quitter l'application." "\n"
            "W/A/S/D : déplacer la caméra horizontalement." "\n"
            "Q/E : déplacer la caméra verticalement." "\n"
            "Flèches : tourner la caméra." "\n"
            "Souris : tourner la caméra (avec Espace)." "\n"
            "Espace : activer/désactiver la souris." "\n"
        );

        glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        basicShader_.create();
        backgroundShader_.create();
        splineShader_.create();

        glGenVertexArrays(1, &emptyVao_);

        glGenVertexArrays(1, &splineVao_);
        glGenBuffers(1, &splineVbo_);
        glBindVertexArray(splineVao_);
        glBindBuffer(GL_ARRAY_BUFFER, splineVbo_);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);

        sword_.load("../models/sword.ply");
        staff_.load("../models/staff.ply");

        swordTexture_.load("../textures/Longsword_10_low_Longsword_10_BaseColor.jpg");
        swordTexture_.setFiltering(GL_LINEAR);
        swordTexture_.enableMipmap();

        staffTexture_.load("../textures/Staff.png");
        staffTexture_.setFiltering(GL_LINEAR);
        staffTexture_.enableMipmap();

        swordT_.position = vec3(-1.5f, 0.0f, 0.0f);
        swordT_.scale = 0.5f;

        staffT_.position = vec3(1.5f, 0.0f, 0.0f);
        staffT_.scale = 0.5f;
    }

    void drawFrame() override
    {
        totalTime_ += deltaTime_;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Background: fullscreen gradient, no depth
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        backgroundShader_.use();
        glUniform1f(backgroundShader_.timeULoc, totalTime_);
        glBindVertexArray(emptyVao_);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        updateCameraInput();

        ImGui::Begin("Scene");
        ImGui::Text("Camera: %.2f %.2f %.2f", cameraPosition_.x, cameraPosition_.y, cameraPosition_.z);
        if (ImGui::Button("Reset camera"))
        {
            cameraPosition_ = vec3(0.f, 1.5f, 5.f);
            cameraOrientation_ = vec2(0.f);
        }
        ImGui::Separator();

        ImGui::Text("Sword");
        ImGui::PushID("sword");
        ImGui::SliderFloat3("Position##s", value_ptr(swordT_.position), -20.0f, 20.0f);
        ImGui::SliderFloat3("Rotation##s", value_ptr(swordT_.rotation), -180.0f, 180.0f);
        ImGui::SliderFloat("Scale##s", &swordT_.scale, 0.01f, 10.0f);
        ImGui::PopID();
        ImGui::Separator();

        ImGui::Text("Staff");
        ImGui::PushID("staff");
        ImGui::SliderFloat3("Position##t", value_ptr(staffT_.position), -20.0f, 20.0f);
        ImGui::SliderFloat3("Rotation##t", value_ptr(staffT_.rotation), -180.0f, 180.0f);
        ImGui::SliderFloat("Scale##t", &staffT_.scale, 0.01f, 10.0f);
        ImGui::PopID();
        ImGui::End();

        mat4 view = getViewMatrix();
        mat4 proj = getProjectionMatrix();
        mat4 projView = proj * view;

        basicShader_.use();
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(basicShader_.diffuseSamplerULoc, 0);

        {
            mat4 mvp = projView * swordT_.matrix();
            glUniformMatrix4fv(basicShader_.mvpULoc, 1, GL_FALSE, value_ptr(mvp));
            swordTexture_.use();
            sword_.draw();
        }
        {
            mat4 mvp = projView * staffT_.matrix();
            glUniformMatrix4fv(basicShader_.mvpULoc, 1, GL_FALSE, value_ptr(mvp));
            staffTexture_.use();
            staff_.draw();
        }

        // Animated Bézier spline connecting the two objects
        drawSpline(projView);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    void drawSpline(const mat4& projView)
    {
        vec3 p0 = swordT_.position;
        vec3 p3 = staffT_.position;

        vec3 mid = (p0 + p3) * 0.5f;
        float sway = sin(totalTime_ * 1.5f) * 1.2f;
        float lift = 2.0f + cos(totalTime_ * 1.1f) * 0.6f;

        vec3 c1 = mid + vec3(-1.0f, lift, sway);
        vec3 c2 = mid + vec3( 1.0f, lift, -sway);

        const int N = 64;
        std::vector<float> data;
        data.reserve((N + 1) * 4);
        for (int i = 0; i <= N; ++i)
        {
            float t = float(i) / float(N);
            float u = 1.0f - t;
            vec3 p = u * u * u * p0
                   + 3.0f * u * u * t * c1
                   + 3.0f * u * t * t * c2
                   + t * t * t * p3;
            data.push_back(p.x);
            data.push_back(p.y);
            data.push_back(p.z);
            data.push_back(t);
        }

        glBindBuffer(GL_ARRAY_BUFFER, splineVbo_);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

        splineShader_.use();
        glUniformMatrix4fv(splineShader_.mvpULoc, 1, GL_FALSE, value_ptr(projView));
        glUniform1f(splineShader_.timeULoc, totalTime_);

        glLineWidth(3.0f);
        glDisable(GL_CULL_FACE);
        glBindVertexArray(splineVao_);
        glDrawArrays(GL_LINE_STRIP, 0, N + 1);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }

    void onClose() override {}

    void onKeyPress(const sf::Event::KeyPressed& key) override
    {
        using enum sf::Keyboard::Key;
        switch (key.code)
        {
        case Escape:
            window_.close();
            break;
        case Space:
            isMouseMotionEnabled_ = !isMouseMotionEnabled_;
            window_.setMouseCursorGrabbed(isMouseMotionEnabled_);
            window_.setMouseCursorVisible(!isMouseMotionEnabled_);
            if (isMouseMotionEnabled_)
            {
                sf::Vector2u s = window_.getSize();
                sf::Mouse::setPosition(sf::Vector2i(s.x / 2, s.y / 2), window_);
                skipNextMouseMove_ = true;
            }
            break;
        default: break;
        }
    }

    void onResize(const sf::Event::Resized& event) override {}

    void onMouseMove(const sf::Event::MouseMoved& e) override {}

    void updateCameraInput()
    {
        if (!window_.hasFocus())
            return;

        if (isMouseMotionEnabled_)
        {
            sf::Vector2u s = window_.getSize();
            sf::Vector2i center(int(s.x) / 2, int(s.y) / 2);
            sf::Vector2i cur = sf::Mouse::getPosition(window_);
            int dx = cur.x - center.x;
            int dy = cur.y - center.y;

            if (!skipNextMouseMove_)
            {
                const float SENS = 0.003f;
                cameraOrientation_.y -= dx * SENS;
                cameraOrientation_.x -= dy * SENS;
                cameraOrientation_.x = glm::clamp(cameraOrientation_.x, -1.55f, 1.55f);
            }
            skipNextMouseMove_ = false;
            sf::Mouse::setPosition(center, window_);
        }

        const float ROT_SPEED = 1.5f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    cameraOrientation_.x += ROT_SPEED * deltaTime_;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  cameraOrientation_.x -= ROT_SPEED * deltaTime_;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  cameraOrientation_.y += ROT_SPEED * deltaTime_;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) cameraOrientation_.y -= ROT_SPEED * deltaTime_;

        vec3 offset(0.0f);
        const float SPEED = 5.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) offset.z -= SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) offset.z += SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) offset.x -= SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) offset.x += SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) offset.y -= SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) offset.y += SPEED;

        offset = vec3(rotate(mat4(1.0f), cameraOrientation_.y, vec3(0, 1, 0)) * vec4(offset, 0.0f));
        cameraPosition_ += offset * deltaTime_;
    }

    mat4 getViewMatrix()
    {
        mat4 view(1.0f);
        view = rotate(view, -cameraOrientation_.x, vec3(1, 0, 0));
        view = rotate(view, -cameraOrientation_.y, vec3(0, 1, 0));
        view = translate(view, -cameraPosition_);
        return view;
    }

    mat4 getProjectionMatrix()
    {
        auto size = window_.getSize();
        float aspect = float(size.x) / float(size.y);
        return perspective(radians(70.0f), aspect, 0.1f, 200.0f);
    }

private:
    BasicShader basicShader_;
    BackgroundShader backgroundShader_;
    SplineShader splineShader_;

    Model sword_;
    Model staff_;

    Texture2D swordTexture_;
    Texture2D staffTexture_;

    Transform swordT_;
    Transform staffT_;

    GLuint emptyVao_ = 0;
    GLuint splineVao_ = 0;
    GLuint splineVbo_ = 0;

    float totalTime_;

    vec3 cameraPosition_;
    vec2 cameraOrientation_;
    bool isMouseMotionEnabled_;
    bool skipNextMouseMove_ = false;
};


int main(int argc, char* argv[])
{
    WindowSettings settings = {};
    settings.fps = 60;
    settings.context.depthBits = 24;
    settings.context.stencilBits = 8;
    settings.context.antiAliasingLevel = 4;
    settings.context.majorVersion = 3;
    settings.context.minorVersion = 3;
    settings.context.attributeFlags = sf::ContextSettings::Attribute::Core;

    App app;
    app.run(argc, argv, "Tp4 - Sword 'n Magic", settings);
}
