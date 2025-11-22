// #include "CubeMapTexture.h"
// //#define STB_IMAGE_IMPLEMENTATION
// #include "stb/stb_image.h"

// #include <iostream>

// const GLenum TARGETS[] = {
//     GL_TEXTURE_CUBE_MAP_POSITIVE_X, // posx.png
//     GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // negx.png
//     GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // posy.png
//     GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // negy.png
//     GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // posz.png
//     GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // negz.png
// };

// CubeMapTexture::CubeMapTexture(const std::vector<std::string>& faces) 
//     : textureID(0) 
// {
//     loadCubeMap(faces);
// }

// CubeMapTexture::~CubeMapTexture() {
//     if (textureID != 0)
//         glDeleteTextures(1, &textureID);
// }

// void CubeMapTexture::bind(GLuint unit) const {
//     glActiveTexture(GL_TEXTURE0 + unit);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
// }

// void CubeMapTexture::unbind() const {
//     glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
// }

// void CubeMapTexture::loadCubeMap(const std::vector<std::string>& faces) {
//     glGenTextures(1, &textureID);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

//     int width, height, nrChannels;

//     stbi_set_flip_vertically_on_load(false);

//     for (unsigned int i = 0; i < faces.size(); i++) {
//         unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        
//         if (data) {
//             GLenum format = GL_RGB;
//             if (nrChannels == 4) format = GL_RGBA;
            
//             glTexImage2D(TARGETS[i], 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//             stbi_image_free(data);
//         } else {
//             std::cerr << "CubeMap neuspěla při načítání souboru: " << faces[i] << std::endl;
//             stbi_set_flip_vertically_on_load(true);
//             glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//             return;
//         }
//     }
    
//     stbi_set_flip_vertically_on_load(true); 

//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

//     glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
// }