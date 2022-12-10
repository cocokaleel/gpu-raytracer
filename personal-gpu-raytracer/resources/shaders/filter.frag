#version 330 core

// UV coordinate in variable representing a pixel in the full-screen quad
in vec2 uvCoords;

// Task 8: Add a sampler2D uniform
uniform sampler2D rendered_geometry_sampler;
uniform int image_width;
uniform int image_height;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool perPixelFilter;
uniform bool perPixelExtraCreditFilter;
uniform bool kernelFilter;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1);
    // Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(rendered_geometry_sampler, uvCoords);

    //apply kernel-based transformation first, as it samples the original colors
    if (kernelFilter) {
        //blur filter
        vec4 runningSum = vec4(0);

        //a pixel-to-pixel unit distance in the UV coordinate grid
        float dx = 1.f/image_width;
        float dy = 1.f/image_height;

        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                float u = min(max(uvCoords[0]+i*dx,0), 1.f-dx); //clamps the sampling to pixels within the uv grid
                float v = min(max(uvCoords[1]+j*dy,0), 1.f-dy);
                vec2 uv = vec2(u,v);
                runningSum += (1.f/25.f)*texture(rendered_geometry_sampler, uv);
            }
        }

        fragColor = runningSum;
    }

    //Apply per-pixel transformations, if requested
    float luma = fragColor[0]*0.299+ fragColor[1]*0.587+ fragColor[2]*0.144;
    fragColor = perPixelFilter ? vec4(vec3(luma), 1.f) : fragColor;

    fragColor = perPixelExtraCreditFilter ? 1.f-fragColor:fragColor;

}
