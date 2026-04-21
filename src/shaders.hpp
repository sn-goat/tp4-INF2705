#pragma once

#include "shader_program.hpp"

class BasicShader : public ShaderProgram
{
public:
    GLint mvpULoc = -1;
    GLint diffuseSamplerULoc = -1;

protected:
    void load() override
    {
        name_ = "BasicShader";
        loadShaderSource(GL_VERTEX_SHADER, "./shaders/basic.vs.glsl");
        loadShaderSource(GL_FRAGMENT_SHADER, "./shaders/basic.fs.glsl");
        link();
    }

    void getAllUniformLocations() override
    {
        mvpULoc = glGetUniformLocation(id_, "mvp");
        diffuseSamplerULoc = glGetUniformLocation(id_, "diffuseSampler");
    }
};

class BackgroundShader : public ShaderProgram
{
public:
    GLint timeULoc = -1;

protected:
    void load() override
    {
        name_ = "BackgroundShader";
        loadShaderSource(GL_VERTEX_SHADER, "./shaders/background.vs.glsl");
        loadShaderSource(GL_FRAGMENT_SHADER, "./shaders/background.fs.glsl");
        link();
    }

    void getAllUniformLocations() override
    {
        timeULoc = glGetUniformLocation(id_, "time");
    }
};

class SplineShader : public ShaderProgram
{
public:
    GLint mvpULoc = -1;
    GLint timeULoc = -1;

protected:
    void load() override
    {
        name_ = "SplineShader";
        loadShaderSource(GL_VERTEX_SHADER, "./shaders/spline.vs.glsl");
        loadShaderSource(GL_FRAGMENT_SHADER, "./shaders/spline.fs.glsl");
        link();
    }

    void getAllUniformLocations() override
    {
        mvpULoc = glGetUniformLocation(id_, "mvp");
        timeULoc = glGetUniformLocation(id_, "time");
    }
};
