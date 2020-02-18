attribute highp vec4 a_position;
attribute highp vec2 a_texcoord;
attribute highp vec3 a_normal;
uniform highp mat4 u_projectionMatrix;
uniform highp mat4 u_viewMatrix;
uniform highp mat4 u_modelMatrix;
varying highp vec4 v_position;
varying highp vec2 v_texcoord;
varying highp vec3 v_normal;

void main(void)
{
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
    v_texcoord = a_texcoord;
    v_normal=normalize(vec3(u_viewMatrix * u_modelMatrix *vec4(a_normal,0.0)));
    v_position=u_viewMatrix * u_modelMatrix * a_position;
}
