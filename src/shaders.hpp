#pragma once

#include "shader_program.hpp"

class BasicShader : public ShaderProgram
{
public:
    GLint mvpULoc = -1;
    GLint modelViewULoc = -1;
    GLint normalMatrixULoc = -1;
    GLint diffuseSamplerULoc = -1;

    GLint ambientColorULoc = -1;
    GLint dirLightDirViewULoc = -1;
    GLint dirLightColorULoc = -1;
    GLint pointLightPosViewULoc = -1;
    GLint pointLightColorULoc = -1;

    GLint kaULoc = -1;
    GLint kdULoc = -1;
    GLint ksULoc = -1;
    GLint shininessULoc = -1;

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
        modelViewULoc = glGetUniformLocation(id_, "modelView");
        normalMatrixULoc = glGetUniformLocation(id_, "normalMatrix");
        diffuseSamplerULoc = glGetUniformLocation(id_, "diffuseSampler");

        ambientColorULoc = glGetUniformLocation(id_, "ambientColor");
        dirLightDirViewULoc = glGetUniformLocation(id_, "dirLightDirView");
        dirLightColorULoc = glGetUniformLocation(id_, "dirLightColor");
        pointLightPosViewULoc = glGetUniformLocation(id_, "pointLightPosView");
        pointLightColorULoc = glGetUniformLocation(id_, "pointLightColor");

        kaULoc = glGetUniformLocation(id_, "ka");
        kdULoc = glGetUniformLocation(id_, "kd");
        ksULoc = glGetUniformLocation(id_, "ks");
        shininessULoc = glGetUniformLocation(id_, "shininess");
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

class ColorShader : public ShaderProgram
{
public:
    GLint mvpULoc = -1;
    GLint colorULoc = -1;

protected:
    void load() override
    {
        name_ = "ColorShader";
        loadShaderSource(GL_VERTEX_SHADER, "./shaders/color.vs.glsl");
        loadShaderSource(GL_FRAGMENT_SHADER, "./shaders/color.fs.glsl");
        link();
    }

    void getAllUniformLocations() override
    {
        mvpULoc = glGetUniformLocation(id_, "mvp");
        colorULoc = glGetUniformLocation(id_, "color");
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
