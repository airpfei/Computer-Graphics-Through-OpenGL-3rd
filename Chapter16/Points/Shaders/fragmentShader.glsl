#version 430 core

uniform vec4 pointColor;
uniform sampler2D starTex;
uniform uint renderChoice;

out vec4 colorsOut;

const vec4 backgroundColor = vec4(1.0, 1.0, 1.0, 0.0);

vec2 coordWRTcenter;
float distFromCenter, alpha;
float startBlend = 0.475;

void main(void)
{
   switch(renderChoice)
   {
      case 0:
	     colorsOut = pointColor;
		 break;
	  case 1:
         coordWRTcenter = gl_PointCoord - vec2(0.5); 
         distFromCenter = sqrt(dot(coordWRTcenter, coordWRTcenter));
         if (distFromCenter > 0.5) discard;	  
		 colorsOut = pointColor;
		 break;
	  case 2:
         coordWRTcenter = gl_PointCoord - vec2(0.5);
         distFromCenter = sqrt(dot(coordWRTcenter, coordWRTcenter));
         if (distFromCenter > 0.5) discard;
         alpha = clamp( (distFromCenter - startBlend)/
		                (0.5 - startBlend), 0.0, 1.0 );
         colorsOut = mix(pointColor, backgroundColor, alpha);
		 break;
	  case 3:
	     colorsOut = texture(starTex, gl_PointCoord);
		 break;
      default:
         break;
   }
}