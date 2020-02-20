uniform sampler2D u_texture;
uniform highp vec4 u_lightPosition;
uniform highp float u_lightPower;
uniform highp float u_alpha;
uniform highp float u_r;
uniform highp float u_g;
uniform highp float u_b;
varying highp vec4 v_position;
varying highp vec2 v_texcoord;
varying highp vec3 v_normal;

void main(void)
{
    vec4 resultColor=vec4(0.0,0.0,0.0,0.0);
    vec4 eyePosition=vec4(0.0,0.0,0.0,1.0);
    vec4 diffMatColor;
    if(u_r==0. && u_g==0. && u_b==0.)
    {
        diffMatColor=texture2D(u_texture, v_texcoord);
    }
    else
    {
        diffMatColor=vec4(u_r*0.2,u_g*0.2,u_b*0.2,1.0);
    }

    vec3 eyeVect=normalize(v_position.xyz-eyePosition.xyz);
    vec3 lightVect=normalize(v_position.xyz-u_lightPosition.xyz);
    vec3 reflectLight=normalize(reflect(lightVect,v_normal));
    float len=length(v_position.xyz-eyePosition.xyz);
    vec4 diffColor=diffMatColor*u_lightPower*dot(v_normal,-lightVect)/(1.0+0.25*pow(len,2.0));
    resultColor+=diffColor;
    gl_FragColor = resultColor;
    gl_FragColor.w = u_alpha;
}
