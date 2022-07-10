#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

// td3
varying vec4 varyColor; //Sometimes we use "out" instead of "varying". "out" should be used in later version of GLSL.

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main()
{
    gl_FragColor = varyColor;
}

/*
//TD4

varying vec4 varyColor;

uniform vec4 uMltCts;
uniform vec3 uMltColor;

uniform vec3 uLightPosition;
uniform vec3 uLightColor;

uniform vec4 uCameraPos;

varying vec3 vary_normal;
varying vec4 vary_world_position;

void main(){

}
*/