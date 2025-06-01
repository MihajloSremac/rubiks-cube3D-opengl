#version 330 core
        out vec4 FragColor;
        uniform vec4 cubeColor;
        
        void main() {
            FragColor = cubeColor;
        }