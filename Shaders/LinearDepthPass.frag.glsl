#version 330

#ifdef ALPHA_TEST_ON
    // Use the main texture and texcoord for alpha testing
    uniform sampler2D _MainTexture;
    in vec2 texcoord;
#endif

in float linearDistance;
out float fragColor;

void main()
{
#ifdef ALPHA_TEST_ON
    // Discard fragment if main texture alpha is too low.
    if(texture(_MainTexture, texcoord).a < 0.5) discard;
#endif
    
    // Write out the linear distance to the fragment
    fragColor = linearDistance;
}
